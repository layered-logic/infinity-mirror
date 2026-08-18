#include "stm8s.h"

// =================== Hardware ===================
#define NUM_LEDS 32

// LED data on PC6 (custom PCB, TSSOP20 pin 16).
// Corrected 2026-08-17: this said PC7/pin 17, but the PCB has routed
// /LED_SIG from PC6 since the 5bc9107 baseline -- verified across all three
// board revisions. On PC7 the strip simply stayed dark.
#define DATA_PORT GPIOC
#define DATA_MASK GPIO_PIN_6

// Mode button on PD6 (TSSOP20 pin 3), active low. The board carries a 10k
// pull-up and a 100n debounce cap -- which is also why UART1_RX is unusable
// on this design: RX is physically PD6 and it is already spoken for.
#define BTN_PORT GPIOD
#define BTN_MASK GPIO_PIN_6

// ---- rework pass 2 hardware, 2026-08-17 (see the board's REWORK_SPEC.md) ----
// USB-C CC sense: /CC1 -> R7 10k -> PD2 (AIN3), /CC2 -> R8 10k -> PD3 (AIN4).
#define CC1_ADC_CH 3
#define CC2_ADC_CH 4
// UART1_TX debug pad TP1 on PD5 (pin 2). TP2 = GND, TP3 = PD4 (spare).
#define ENABLE_UART 1
#define UART_BAUD_DIV 139           /* 16 MHz / 115200 = 138.9 */
// IR receiver J8 (fitted DNP) OUT -> PA3 (pin 10) = TIM2_CH3.
// Set to 0 to reclaim the flash if J8 is never populated.
#define ENABLE_IR 1
// Placeholder NEC command bytes -- these are remote-specific. Every received
// code is printed on TP1, so capture your remote's actual bytes first and
// then correct these. Left as distinct dummy values so an unknown remote
// simply does nothing instead of firing the wrong action.
#define IR_CMD_POWER     0x45
#define IR_CMD_COLOR     0x46
#define IR_CMD_PATTERN   0x47
#define IR_CMD_BRIGHT_UP 0x40
#define IR_CMD_BRIGHT_DN 0x19

// CC decode. There is NO regulator on this board -- VDD is raw VBUS -- so the
// ADC is RATIOMETRIC and thresholds must be fractions of full scale, never
// absolute volts. With Rd = 5.1k the CC node sits at 0.41 V (default USB),
// 0.92 V (1.5 A) or 1.68 V (3.0 A); across VDD 4.75-5.5 V those land at codes
// 76-88, 171-198 and 313-362, so the gaps are wide and these sit mid-gap.
#define CC_TH_1A5 130
#define CC_TH_3A0 255

// Brightness ceiling per advertised source tier. 32 WS2812 at full white draw
// roughly 32 x 60 mA = 1.9 A, so each cap is (budget / 1.9 A) scaled to 255.
#define BRIGHT_CAP_DEFAULT 90       /* <=0.9 A source -> ~0.7 A budget */
#define BRIGHT_CAP_1A5 160          /* 1.5 A source   -> ~1.2 A budget */
#define BRIGHT_CAP_3A0 255          /* 3.0 A source   -> unrestricted  */
#define CC_POLL_TICKS 20            /* re-read CC about once a second */

// =================== Settings ===================
#define DEFAULT_BRIGHTNESS 128  // 0..255
#define MIN_BRIGHTNESS 25       // Minimum brightness (about 10%)
#define BRIGHTNESS_STEP 25      // 10% of 255 ~= 25
#define DEBOUNCE_MS 20          // Reduced - hardware cap handles most bounce
#define DOUBLE_CLICK_MS 200     // Max time between clicks for double/triple-click
#define LONG_PRESS_MS 600       // Hold time for long press
#define PATTERN_STEP_MS 50      // Pattern animation speed
#define WHITE_DIM_FACTOR 217    // 85% of 255 = ~217 (15% dimmer for white)

// =================== EEPROM Addresses ===================
#define EEPROM_BASE       0x4000
#define EEPROM_MAGIC_ADDR (EEPROM_BASE + 0)
#define EEPROM_COLOR_ADDR (EEPROM_BASE + 1)
#define EEPROM_PATTERN_ADDR (EEPROM_BASE + 2)
#define EEPROM_BRIGHTNESS_ADDR (EEPROM_BASE + 3)
#define EEPROM_MAGIC_VALUE 0xA5  // Indicates valid saved state

// =================== Color Definitions ===================
// Colors: white, red, red-orange, orange, yellow-orange, yellow,
//         yellow-green, green, blue-green, blue, blue-purple, purple, red-purple
#define NUM_COLORS 13

typedef struct {
  uint8_t r, g, b;
} RGB;

static const RGB colors[NUM_COLORS] = {
  {255, 255, 255},  // White
  {255, 0, 0},      // Red
  {255, 64, 0},     // Red-Orange
  {255, 128, 0},    // Orange
  {255, 192, 0},    // Yellow-Orange
  {255, 255, 0},    // Yellow
  {128, 255, 0},    // Yellow-Green
  {0, 255, 0},      // Green
  {0, 255, 128},    // Blue-Green
  {0, 0, 255},      // Blue
  {64, 0, 255},     // Blue-Purple
  {128, 0, 255},    // Purple
  {255, 0, 128}     // Red-Purple
};

// =================== Pattern Definitions ===================
#define NUM_PATTERNS 7
typedef enum {
  PATTERN_SOLID = 0,
  PATTERN_RAINBOW,
  PATTERN_SCANNER,
  PATTERN_SPINNER,
  PATTERN_RANDOM,
  PATTERN_BREATHING,
  PATTERN_TWINKLE
} Pattern;

// =================== State ===================
static uint8_t isOn = 0;
static uint8_t currentColor = 0;
static uint8_t currentPattern = PATTERN_SOLID;
static uint8_t currentBrightness = DEFAULT_BRIGHTNESS;
static uint8_t animStep = 0;
static uint8_t breathDir = 1;
static uint8_t breathVal = 0;

// LED buffer for patterns
static RGB ledBuffer[NUM_LEDS];

// Random state
static uint16_t rngState = 12345;

// =================== Utilities ===================
static uint8_t scale8(uint8_t v, uint8_t sc) {
  return (uint16_t)v * sc / 255;
}

static void delay_ms_soft(uint16_t ms) {
  while (ms--) for (volatile uint16_t i = 0; i < 1600; i++) __asm__("nop");
}

static uint8_t random8(void) {
  // Simple 16-bit LFSR for STM8
  uint8_t lsb;
  lsb = rngState & 1;
  rngState >>= 1;
  if (lsb) {
    rngState ^= 0xB400;  // Taps for maximal length
  }
  return (uint8_t)rngState;
}

// =================== EEPROM Functions ===================
static void eeprom_unlock(void) {
  FLASH->DUKR = 0xAE;  // First key
  FLASH->DUKR = 0x56;  // Second key
  while (!(FLASH->IAPSR & FLASH_IAPSR_DUL));  // Wait for unlock
}

static void eeprom_lock(void) {
  FLASH->IAPSR &= (uint8_t)~FLASH_IAPSR_DUL;
}

static void eeprom_write(uint16_t addr, uint8_t data) {
  eeprom_unlock();
  *((uint8_t *)addr) = data;
  while (!(FLASH->IAPSR & FLASH_IAPSR_EOP));  // Wait for write complete
  eeprom_lock();
}

static uint8_t eeprom_read(uint16_t addr) {
  return *((uint8_t *)addr);
}

static void saveState(void) {
  eeprom_write(EEPROM_COLOR_ADDR, currentColor);
  eeprom_write(EEPROM_PATTERN_ADDR, currentPattern);
  eeprom_write(EEPROM_BRIGHTNESS_ADDR, currentBrightness);
  eeprom_write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
}

static void clearSavedState(void) {
  // Invalidate saved state - power loss will reset to defaults
  eeprom_write(EEPROM_MAGIC_ADDR, 0x00);
}

static uint8_t loadState(void) {
  // Returns 1 if state was loaded, 0 if using defaults
  if (eeprom_read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    currentColor = eeprom_read(EEPROM_COLOR_ADDR);
    currentPattern = eeprom_read(EEPROM_PATTERN_ADDR);
    currentBrightness = eeprom_read(EEPROM_BRIGHTNESS_ADDR);
    // Validate loaded values
    if (currentColor >= NUM_COLORS) currentColor = 0;
    if (currentPattern >= NUM_PATTERNS) currentPattern = 0;
    if (currentBrightness < MIN_BRIGHTNESS) currentBrightness = DEFAULT_BRIGHTNESS;
    // Clear after loading so power loss won't restore this state again
    clearSavedState();
    return 1;
  }
  return 0;
}

// =================== USB-C CC sense (ADC) ===================
// PD2/PD3 MUST stay input-floating (CR1 clear). An internal pull-up would
// corrupt the CC divider and misread even a good 3 A source -- not a silent
// failure, so never blanket-init these two pins as pull-up inputs.
static uint8_t ccTier = 0;                   /* 0 = default, 1 = 1.5A, 2 = 3.0A */
static uint8_t ccMaxBright = BRIGHT_CAP_DEFAULT;

static void adc_init(void) {
  volatile uint8_t d;
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_2 | GPIO_PIN_3),
            GPIO_MODE_IN_FL_NO_IT);
  ADC1->CR1 = 0x00;                  /* fmaster/2, single conversion mode */
  ADC1->CR2 = ADC1_CR2_ALIGN;        /* right aligned -> read DRL before DRH */
  ADC1->CR1 |= ADC1_CR1_ADON;        /* the first ADON only wakes the ADC */
  for (d = 0; d < 60; d++) __asm__("nop");   /* tSTAB = 7 us at 16 MHz */
}

static uint16_t adc_read(uint8_t ch) {
  uint8_t i;
  uint16_t v = 0;
  ADC1->CSR = ch;                    /* select channel, clear EOC, no interrupt */
  /* Two back-to-back conversions, keep the second. Source impedance is
     10k + 5.1k ~= 15.1k against the 3 pF sample-and-hold: ~8.3 time constants
     inside the 0.75 us sample window, so ONE conversion is already good to
     ~0.3 LSB. (At the 22k this started life as it was 4.6 tau and ~10 LSB,
     and the second read was mandatory.) Kept as belt-and-braces -- it costs
     a few microseconds and makes the result independent of whatever else
     the ADC is later used for. */
  for (i = 0; i < 2; i++) {
    ADC1->CR1 |= ADC1_CR1_ADON;      /* the second ADON starts a conversion */
    while (!(ADC1->CSR & ADC1_CSR_EOC));
    v = (uint16_t)ADC1->DRL;         /* DRL FIRST when right-aligned */
    v |= (uint16_t)((uint16_t)ADC1->DRH << 8);
    ADC1->CSR &= (uint8_t)~ADC1_CSR_EOC;
  }
  return v;
}

static void cc_poll(void) {
  uint16_t a, b, cc;
  a = adc_read(CC1_ADC_CH);
  b = adc_read(CC2_ADC_CH);
  cc = (a > b) ? a : b;              /* the mated orientation is the higher one */
  if (cc >= CC_TH_3A0)      ccTier = 2;
  else if (cc >= CC_TH_1A5) ccTier = 1;
  else                      ccTier = 0;
  ccMaxBright = (ccTier == 2) ? BRIGHT_CAP_3A0
              : (ccTier == 1) ? BRIGHT_CAP_1A5 : BRIGHT_CAP_DEFAULT;
}

// =================== UART1 debug: TP1 = PD5 (TX), TP2 = GND ===================
#if ENABLE_UART
static void uart_init(void) {
  /* UART1 drives PD5 itself once TEN is set. Transmit only -- RX would be
     PD6, which is the Mode button. */
  UART1->CR1 = 0x00;
  UART1->CR3 = 0x00;                 /* 1 stop bit */
  UART1->BRR2 = (uint8_t)((UART_BAUD_DIV & 0x0F) |
                          ((UART_BAUD_DIV >> 8) & 0xF0));
  UART1->BRR1 = (uint8_t)((UART_BAUD_DIV >> 4) & 0xFF);  /* BRR1 last: it latches */
  UART1->CR2 = UART1_CR2_TEN;
}

static void uart_putc(char c) {
  while (!(UART1->SR & UART1_SR_TXE));
  UART1->DR = (uint8_t)c;
}

static void uart_puts(const char *s) {
  while (*s) uart_putc(*s++);
}

static void uart_putu16(uint16_t v) {
  char buf[6];
  uint8_t i = 5;
  buf[5] = 0;
  if (!v) { uart_putc('0'); return; }
  while (v && i) { buf[--i] = (char)('0' + (uint8_t)(v % 10)); v /= 10; }
  uart_puts(&buf[i]);
}
#else
#define uart_init()
#define uart_puts(s)
#define uart_putu16(v)
#endif

// =================== IR receiver: J8 (DNP) OUT -> PA3 ===================
#if ENABLE_IR
/* PA3 = TIM2_CH3 as the DEFAULT alternate function. Leave option byte AFR1
   UNPROGRAMMED: it would remap TIM2_CH3 to PD2, which is now AIN3, and the
   datasheet's bracket notation means an exclusive choice, not a duplicate.

   NEC framing, measured falling edge to falling edge with TIM2 at 8 us/tick:
   leader 13.5 ms, '0' 1.125 ms, '1' 2.25 ms. showLEDs() disables interrupts
   for about 1 ms per 50 ms frame, so an edge is occasionally lost; NEC
   remotes repeat, so a dropped frame is not worth extra machinery. */
#define IR_LEADER_MIN 1400
#define IR_LEADER_MAX 1900
#define IR_ZERO_MIN     90
#define IR_ZERO_MAX    190
#define IR_ONE_MIN     220
#define IR_ONE_MAX     350

static volatile uint32_t irShift = 0;
static volatile uint16_t irLast = 0;
static volatile uint8_t irBits = 0;
static volatile uint8_t irCmd = 0;
static volatile uint8_t irReady = 0;

static void ir_init(void) {
  /* Pulled up so the pin does not float on boards where J8 is not fitted. */
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
  TIM2->PSCR = 7;                    /* 16 MHz / 128 = 8 us per tick */
  TIM2->CCMR3 = 0x01;                /* CC3 = input, mapped to TI3 */
  TIM2->CCER2 = 0x03;                /* CC3E | CC3P -> capture on falling edge */
  TIM2->IER = TIM2_IER_CC3IE;
  TIM2->CR1 = 0x01;                  /* counter on, free running */
}

INTERRUPT_HANDLER(TIM2_CAPCOM_IRQHandler, 14) {
  uint16_t now, d;
  uint8_t cmd, inv;
  if (!(TIM2->SR1 & TIM2_SR1_CC3IF)) return;
  now = (uint16_t)(((uint16_t)TIM2->CCR3H << 8) | TIM2->CCR3L);
  TIM2->SR1 = (uint8_t)~TIM2_SR1_CC3IF;
  d = (uint16_t)(now - irLast);      /* 16-bit wrap is intentional */
  irLast = now;
  if (d >= IR_LEADER_MIN && d <= IR_LEADER_MAX) {
    irShift = 0; irBits = 0;
  } else if (d >= IR_ZERO_MIN && d <= IR_ZERO_MAX) {
    irShift <<= 1; irBits++;
  } else if (d >= IR_ONE_MIN && d <= IR_ONE_MAX) {
    irShift = (irShift << 1) | 1UL; irBits++;
  } else {
    irBits = 0;                      /* out of spec: resync on the next leader */
  }
  if (irBits >= 32) {
    cmd = (uint8_t)((irShift >> 8) & 0xFF);
    inv = (uint8_t)(irShift & 0xFF);
    if ((uint8_t)(~cmd) == inv) { irCmd = cmd; irReady = 1; }
    irBits = 0;
  }
}
#endif

// =================== WS2812B driver (16MHz STM8) ===================
#define NOP1 __asm__("nop")
#define NOP2 NOP1; NOP1
#define NOP4 NOP2; NOP2
#define NOP10 NOP4; NOP4; NOP2

static void sendByte(uint8_t b) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    if (b & 0x80) {
      DATA_PORT->ODR |= DATA_MASK;
      NOP10; NOP2;
      DATA_PORT->ODR &= (uint8_t)~DATA_MASK;
      NOP4;
    } else {
      DATA_PORT->ODR |= DATA_MASK;
      NOP4;
      DATA_PORT->ODR &= (uint8_t)~DATA_MASK;
      NOP10; NOP2;
    }
    b <<= 1;
  }
}

static void showLEDs(void) {
  uint8_t i;
  volatile uint16_t j;
  uint8_t r, g, b, br;
  disableInterrupts();
  for (i = 0; i < NUM_LEDS; i++) {
    // Apply white dimming if color is white (index 0)
    br = currentBrightness;
    /* Hard ceiling from the USB-C source's advertised current. Applied here,
       not in the button handlers, so nothing can route around it. */
    if (br > ccMaxBright) br = ccMaxBright;
    if (currentColor == 0 && currentPattern == PATTERN_SOLID) {
      br = scale8(br, WHITE_DIM_FACTOR);
    }
    r = scale8(ledBuffer[i].r, br);
    g = scale8(ledBuffer[i].g, br);
    b = scale8(ledBuffer[i].b, br);
    sendByte(g); sendByte(r); sendByte(b);
  }
  enableInterrupts();
  for (j = 0; j < 2500; j++) __asm__("nop");
}

static void setAllLEDs(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t i;
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = r;
    ledBuffer[i].g = g;
    ledBuffer[i].b = b;
  }
}

static void clearLEDs(void) {
  setAllLEDs(0, 0, 0);
  showLEDs();
}

// =================== Color Wheel (for rainbow) ===================
static void wheel(uint8_t pos, uint8_t *r, uint8_t *g, uint8_t *b) {
  if (pos < 85) { *r = 255 - pos*3; *g = pos*3; *b = 0; }
  else if (pos < 170) { pos -= 85; *r = 0; *g = 255 - pos*3; *b = pos*3; }
  else { pos -= 170; *r = pos*3; *g = 0; *b = 255 - pos*3; }
}

// =================== Pattern Functions ===================
static void pattern_solid(void) {
  uint8_t cr = colors[currentColor].r;
  uint8_t cg = colors[currentColor].g;
  uint8_t cb = colors[currentColor].b;
  setAllLEDs(cr, cg, cb);
}

static void pattern_rainbow(void) {
  uint8_t i, pos;
  for (i = 0; i < NUM_LEDS; i++) {
    pos = (i * 256 / NUM_LEDS + animStep) & 0xFF;
    wheel(pos, &ledBuffer[i].r, &ledBuffer[i].g, &ledBuffer[i].b);
  }
  animStep += 3;
}

static void pattern_scanner(void) {
  uint8_t cr, cg, cb, pos, i;
  cr = colors[currentColor].r;
  cg = colors[currentColor].g;
  cb = colors[currentColor].b;
  pos = animStep % (NUM_LEDS * 2);
  if (pos >= NUM_LEDS) pos = (NUM_LEDS * 2) - pos - 1;

  // Fade all LEDs
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = ledBuffer[i].r > 20 ? ledBuffer[i].r - 20 : 0;
    ledBuffer[i].g = ledBuffer[i].g > 20 ? ledBuffer[i].g - 20 : 0;
    ledBuffer[i].b = ledBuffer[i].b > 20 ? ledBuffer[i].b - 20 : 0;
  }

  // Set scanner head with tail
  ledBuffer[pos].r = cr;
  ledBuffer[pos].g = cg;
  ledBuffer[pos].b = cb;

  animStep++;
}

static void pattern_spinner(void) {
  // 4-point spinner - lights at 4 equally spaced positions
  uint8_t cr, cg, cb, spacing, offset, arm, pos, tailPos;
  cr = colors[currentColor].r;
  cg = colors[currentColor].g;
  cb = colors[currentColor].b;
  setAllLEDs(0, 0, 0);

  spacing = NUM_LEDS / 4;
  offset = animStep % spacing;

  for (arm = 0; arm < 4; arm++) {
    pos = (arm * spacing + offset) % NUM_LEDS;
    ledBuffer[pos].r = cr;
    ledBuffer[pos].g = cg;
    ledBuffer[pos].b = cb;

    // Add tail
    tailPos = (pos + NUM_LEDS - 1) % NUM_LEDS;
    ledBuffer[tailPos].r = cr / 3;
    ledBuffer[tailPos].g = cg / 3;
    ledBuffer[tailPos].b = cb / 3;
  }

  animStep++;
}

static void pattern_random(void) {
  uint8_t cr, cg, cb, i, pos;
  cr = colors[currentColor].r;
  cg = colors[currentColor].g;
  cb = colors[currentColor].b;

  // Fade existing
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = ledBuffer[i].r > 15 ? ledBuffer[i].r - 15 : 0;
    ledBuffer[i].g = ledBuffer[i].g > 15 ? ledBuffer[i].g - 15 : 0;
    ledBuffer[i].b = ledBuffer[i].b > 15 ? ledBuffer[i].b - 15 : 0;
  }

  // Light random LEDs
  if ((animStep % 3) == 0) {
    pos = random8() % NUM_LEDS;
    ledBuffer[pos].r = cr;
    ledBuffer[pos].g = cg;
    ledBuffer[pos].b = cb;
  }

  animStep++;
}

static void pattern_breathing(void) {
  uint8_t cr, cg, cb, i;
  cr = colors[currentColor].r;
  cg = colors[currentColor].g;
  cb = colors[currentColor].b;

  // Update breath value
  if (breathDir) {
    if (breathVal < 250) breathVal += 5;
    else breathDir = 0;
  } else {
    if (breathVal > 5) breathVal -= 5;
    else breathDir = 1;
  }

  // Apply breath to all LEDs
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = scale8(cr, breathVal);
    ledBuffer[i].g = scale8(cg, breathVal);
    ledBuffer[i].b = scale8(cb, breathVal);
  }
}

static void pattern_twinkle(void) {
  uint8_t cr, cg, cb, i, rnd;
  cr = colors[currentColor].r;
  cg = colors[currentColor].g;
  cb = colors[currentColor].b;

  // Randomly brighten or dim each LED
  for (i = 0; i < NUM_LEDS; i++) {
    rnd = random8();
    if (rnd < 30) {
      // Sparkle on
      ledBuffer[i].r = cr;
      ledBuffer[i].g = cg;
      ledBuffer[i].b = cb;
    } else {
      // Fade
      ledBuffer[i].r = ledBuffer[i].r > 10 ? ledBuffer[i].r - 10 : 0;
      ledBuffer[i].g = ledBuffer[i].g > 10 ? ledBuffer[i].g - 10 : 0;
      ledBuffer[i].b = ledBuffer[i].b > 10 ? ledBuffer[i].b - 10 : 0;
    }
  }
}

static void updatePattern(void) {
  switch (currentPattern) {
    case PATTERN_SOLID:    pattern_solid(); break;
    case PATTERN_RAINBOW:  pattern_rainbow(); break;
    case PATTERN_SCANNER:  pattern_scanner(); break;
    case PATTERN_SPINNER:  pattern_spinner(); break;
    case PATTERN_RANDOM:   pattern_random(); break;
    case PATTERN_BREATHING: pattern_breathing(); break;
    case PATTERN_TWINKLE:  pattern_twinkle(); break;
  }
  showLEDs();
}

// =================== Button Handling ===================
typedef enum {
  BTN_NONE = 0,
  BTN_SINGLE,
  BTN_DOUBLE,
  BTN_TRIPLE,
  BTN_LONG
} ButtonEvent;

// Button state machine states
typedef enum {
  BS_IDLE = 0,        // Waiting for press
  BS_DEBOUNCE_PRESS,  // Debouncing a press
  BS_PRESSED,         // Button is held down
  BS_DEBOUNCE_REL,    // Debouncing a release
  BS_WAIT_MULTI       // Released, waiting for possible multi-click
} ButtonState;

static uint8_t raw_pressed(void) {
  return (BTN_PORT->IDR & BTN_MASK) ? 0 : 1;
}

static ButtonEvent checkButton(void) {
  static ButtonState state = BS_IDLE;
  static uint8_t clickCount = 0;
  static uint8_t debounceTimer = 0;
  static uint16_t holdTimer = 0;
  static uint16_t multiTimer = 0;

  ButtonEvent event = BTN_NONE;
  uint8_t pressed = raw_pressed();

  switch (state) {
    case BS_IDLE:
      if (pressed) {
        state = BS_DEBOUNCE_PRESS;
        debounceTimer = 0;
      }
      break;

    case BS_DEBOUNCE_PRESS:
      debounceTimer++;
      if (!pressed) {
        // Bounced back - return to idle
        state = BS_IDLE;
      } else if (debounceTimer >= (DEBOUNCE_MS / PATTERN_STEP_MS)) {
        // Stable press confirmed
        state = BS_PRESSED;
        holdTimer = 0;
      }
      break;

    case BS_PRESSED:
      holdTimer++;
      if (!pressed) {
        // Button released
        state = BS_DEBOUNCE_REL;
        debounceTimer = 0;
      } else if (holdTimer >= (LONG_PRESS_MS / PATTERN_STEP_MS)) {
        // Long press detected
        event = BTN_LONG;
        clickCount = 0;
        // Wait for release before going back to idle
        state = BS_DEBOUNCE_REL;
        debounceTimer = 0;
      }
      break;

    case BS_DEBOUNCE_REL:
      debounceTimer++;
      if (pressed) {
        // Bounced back to pressed
        state = BS_PRESSED;
      } else if (debounceTimer >= (DEBOUNCE_MS / PATTERN_STEP_MS)) {
        // Stable release confirmed
        if (holdTimer < (LONG_PRESS_MS / PATTERN_STEP_MS)) {
          // Was a short click, count it
          clickCount++;
          if (clickCount >= 3) {
            event = BTN_TRIPLE;
            clickCount = 0;
            state = BS_IDLE;
          } else {
            state = BS_WAIT_MULTI;
            multiTimer = 0;
          }
        } else {
          // Was a long press (already fired), just go idle
          state = BS_IDLE;
        }
      }
      break;

    case BS_WAIT_MULTI:
      multiTimer++;
      if (pressed) {
        // Another click starting
        state = BS_DEBOUNCE_PRESS;
        debounceTimer = 0;
      } else if (multiTimer >= (DOUBLE_CLICK_MS / PATTERN_STEP_MS)) {
        // Timeout - fire the appropriate event
        if (clickCount == 1) {
          event = BTN_SINGLE;
        } else if (clickCount == 2) {
          event = BTN_DOUBLE;
        }
        clickCount = 0;
        state = BS_IDLE;
      }
      break;
  }

  return event;
}

// =================== Setup / Loop ===================
void setup() {
  // Drive data pin low immediately
  GPIO_Init(DATA_PORT, DATA_MASK, GPIO_MODE_OUT_PP_LOW_FAST);
  DATA_PORT->ODR &= (uint8_t)~DATA_MASK;

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  // Button input pull-up
  GPIO_Init(BTN_PORT, BTN_MASK, GPIO_MODE_IN_PU_NO_IT);

  // TP3 / PD4 is a bare debug pad and floats on every assembled board; the
  // same is true of PA3 when J8 is not fitted (ir_init pulls that one up).
  // Floating CMOS inputs burn current and pick up noise, so tie them off.
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);

  adc_init();
  cc_poll();              // know the source budget before the first frame
  uart_init();
#if ENABLE_IR
  ir_init();
#endif
  enableInterrupts();

  uart_puts("\r\nSTM8 mirror ready. CC tier=");
  uart_putu16(ccTier);
  uart_puts(" cap=");
  uart_putu16(ccMaxBright);
  uart_puts("\r\n");

  // Initialize
  delay_ms_soft(100);
  clearLEDs();
  delay_ms_soft(50);

  isOn = 0;
  // Clear any saved state from previous power session
  // State is only restored within the same power session (long-press off -> single-click on)
  // Unplugging always resets to defaults
  clearSavedState();
}

void loop() {
  ButtonEvent btn;
  static uint8_t ccTick = 0;
  static uint8_t lastTier = 255;

  // Re-read the CC lines about once a second: the cable can be swapped for a
  // weaker charger at any time, and the brightness ceiling has to follow.
  if (++ccTick >= CC_POLL_TICKS) {
    ccTick = 0;
    cc_poll();
    if (ccTier != lastTier) {
      lastTier = ccTier;
      uart_puts("CC tier=");
      uart_putu16(ccTier);
      uart_puts(" cap=");
      uart_putu16(ccMaxBright);
      uart_puts("\r\n");
    }
  }

#if ENABLE_IR
  // Unknown codes are printed rather than ignored: that is how you learn a
  // given remote's map. Point a terminal at TP1 (115200 8N1, ground on TP2),
  // press buttons, then fill in the IR_CMD_* defines below.
  if (irReady) {
    uint8_t c = irCmd;
    irReady = 0;
    uart_puts("IR 0x");
    uart_putu16(c);
    uart_puts("\r\n");
    switch (c) {
      case IR_CMD_POWER:
        if (isOn) { saveState(); isOn = 0; clearLEDs(); }
        else { isOn = 1; loadState(); animStep = 0; breathVal = 0; breathDir = 1; }
        break;
      case IR_CMD_COLOR:
        if (isOn && ++currentColor >= NUM_COLORS) currentColor = 0;
        break;
      case IR_CMD_PATTERN:
        if (isOn) {
          if (++currentPattern >= NUM_PATTERNS) currentPattern = 0;
          animStep = 0; breathVal = 0; breathDir = 1;
          setAllLEDs(0, 0, 0);
        }
        break;
      case IR_CMD_BRIGHT_UP:
        if (isOn && currentBrightness <= (255 - BRIGHTNESS_STEP))
          currentBrightness += BRIGHTNESS_STEP;
        break;
      case IR_CMD_BRIGHT_DN:
        if (isOn && currentBrightness >= (MIN_BRIGHTNESS + BRIGHTNESS_STEP))
          currentBrightness -= BRIGHTNESS_STEP;
        break;
      default:
        break;
    }
  }
#endif

  btn = checkButton();

  switch (btn) {
    case BTN_SINGLE:
      // Single press: turn on or change color
      if (!isOn) {
        isOn = 1;
        // Try to load saved state (from long-press off in same power session)
        // If no saved state, defaults are already set
        loadState();
        animStep = 0;
        breathVal = 0;
        breathDir = 1;
      } else {
        // Cycle to next color
        currentColor++;
        if (currentColor >= NUM_COLORS) {
          currentColor = 0;
        }
      }
      break;

    case BTN_DOUBLE:
      // Double press: cycle pattern
      if (isOn) {
        currentPattern++;
        if (currentPattern >= NUM_PATTERNS) {
          currentPattern = 0;
        }
        animStep = 0;
        breathVal = 0;
        breathDir = 1;
        // Clear buffer for clean pattern start
        setAllLEDs(0, 0, 0);
      }
      break;

    case BTN_TRIPLE:
      // Triple press: reduce brightness by 10%
      if (isOn) {
        if (currentBrightness > MIN_BRIGHTNESS + BRIGHTNESS_STEP) {
          currentBrightness -= BRIGHTNESS_STEP;
        } else {
          // Wrap around to full brightness
          currentBrightness = DEFAULT_BRIGHTNESS;
        }
      }
      break;

    case BTN_LONG:
      // Long press: turn off and save state
      saveState();
      isOn = 0;
      clearLEDs();
      break;

    default:
      break;
  }

  // Update LEDs if on
  if (isOn) {
    updatePattern();
  }

  delay_ms_soft(PATTERN_STEP_MS);
}
