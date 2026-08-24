/*
 * Layered Logic "Basic" -- infinity mirror controller firmware
 * Target: STM8S003F3P6 (TSSOP20), 16 MHz HSI, board rev 2
 *         D:\PCBs\Infinity_Mirror_STM8  (25 x 47 mm, 2-layer, JLC order Y12)
 *
 * Bare-metal SDCC.  No Arduino, no sduino, no SPL object code -- vendor/stm8s.h
 * is included for its register definitions only, so the whole build is
 * `sdcc` + this directory, which is what makes the Pi-side flashing rig
 * reproducible.
 *
 * The user-facing behaviour is unchanged product contract: 13 colours,
 * 7 patterns, single/double/triple/long on one button, EEPROM state that
 * survives an off->on cycle but not a power cycle, and the white auto-dim
 * rule.  See spec.md and docs/pattern-dictionary.md.
 *
 * What changed against Firmware/STM8/stm8_150mm.ino, and why:
 *
 *  1. REAL MILLISECOND TIMEBASE.  The old loop counted iterations of a
 *     calibrated busy-wait and treated each as 50 ms, so every button
 *     threshold was `CONSTANT_MS / PATTERN_STEP_MS` in integer maths --
 *     and DEBOUNCE_MS / PATTERN_STEP_MS is 20/50 = 0, i.e. the debounce
 *     comparisons were always true and there was no debounce at all.  Time
 *     now comes from TIM2's free-running counter at 8 us/tick.  Reading a
 *     counter rather than servicing a tick interrupt matters here: the
 *     WS2812 frame runs with interrupts off for ~1.6 ms at 32 LEDs, which
 *     would have eaten a tick or two every frame.
 *  2. INTERRUPTS THAT ACTUALLY VECTOR.  See src/stm8s_it.h.
 *  3. ADC CLOCK IN SPEC.  fADC was fmaster/2 = 8 MHz; the STM8S003F3
 *     datasheet caps fADC at 4 MHz (6 MHz only above VDDA 4.5 V, which a
 *     sagging USB port does not guarantee).  Now fmaster/6 = 2.67 MHz,
 *     which also widens the sample window to ~1.1 us -- welcome against a
 *     15 k source impedance.
 *  4. CURRENT BUDGET DERIVED FROM NUM_LEDS instead of three magic numbers
 *     hardcoded for a 32-LED strip.  See config.h.
 *  5. NO EEPROM WRITE ON EVERY BOOT.  The old setup() cleared the magic
 *     byte unconditionally, spending a write cycle on each power-up.
 *  6. Hex printed as hex.  The old IR trace printed the code in decimal
 *     behind an "0x" prefix, which is a good way to fill in the wrong
 *     IR_CMD_* constants.
 *  7. Idle pins tied off, and a bring-up mode (ENABLE_BRINGUP) that needs
 *     no button and no remote to prove out a freshly assembled board.
 *
 * Deliberately unchanged: the WS2812 NOP timing (field-proven on shipped
 * 150 mm units) and every pattern algorithm.
 */

#include "stm8s.h"     /* register definitions only -- no SPL object code */
#include "config.h"
#include "board.h"

/* ======================================================================== *
 *  GPIO helpers  (DDR / CR1 / CR2 -- see RM0016 section 11)
 * ======================================================================== */
static void gpio_out_pp_fast(GPIO_TypeDef *p, uint8_t mask) {
  p->DDR |= mask;  p->CR1 |= mask;  p->CR2 |= mask;
}
static void gpio_in_pullup(GPIO_TypeDef *p, uint8_t mask) {
  p->DDR &= (uint8_t)~mask;  p->CR1 |= mask;  p->CR2 &= (uint8_t)~mask;
}
static void gpio_in_float(GPIO_TypeDef *p, uint8_t mask) {
  p->DDR &= (uint8_t)~mask;  p->CR1 &= (uint8_t)~mask;  p->CR2 &= (uint8_t)~mask;
}

/* ======================================================================== *
 *  Timebase -- TIM2 free-running, PSCR 7 => 16 MHz/128 = 8 us per tick
 *
 *  125 ticks == 1 ms exactly, so there is no accumulating rounding error.
 *  tb_poll() must be called at least every 524 ms (the 16-bit wrap); the
 *  main loop calls it thousands of times a second.  The counter keeps
 *  running while interrupts are masked, so the WS2812 frame costs nothing.
 * ======================================================================== */
static uint16_t tb_last;
static uint8_t  tb_frac;          /* leftover 8 us ticks, 0..124 */
static uint16_t g_ms;             /* wraps every 65.5 s; all uses are deltas */

static uint16_t tim2_count(void) {
  uint8_t h = TIM2->CNTRH;        /* reading H latches L */
  uint8_t l = TIM2->CNTRL;
  return (uint16_t)(((uint16_t)h << 8) | l);
}

static void timebase_init(void) {
  TIM2->PSCR = 7;                 /* /128 -> 8 us */
  TIM2->ARRH = 0xFF;
  TIM2->ARRL = 0xFF;
  TIM2->CR1  = TIM2_CR1_CEN;
  tb_last = tim2_count();
  tb_frac = 0;
  g_ms    = 0;
}

static void tb_poll(void) {
  uint16_t now = tim2_count();
  uint16_t d   = (uint16_t)(now - tb_last);   /* 16-bit wrap is intentional */
  uint16_t rem;
  tb_last = now;
  g_ms   += (uint16_t)(d / 125u);
  rem     = (uint16_t)((uint16_t)(d % 125u) + tb_frac);   /* < 250 */
  if (rem >= 125u) { g_ms++; rem = (uint16_t)(rem - 125u); }
  tb_frac = (uint8_t)rem;
}

static void delay_ms(uint16_t ms) {
  uint16_t t0;
  tb_poll();
  t0 = g_ms;
  while ((uint16_t)(g_ms - t0) < ms) tb_poll();
}

/* ======================================================================== *
 *  Small utilities
 * ======================================================================== */
/* v * sc / 255, the FastLED identity: exact at sc = 0 and sc = 255, and one
 * multiply instead of the 16-bit divide the old scale8() paid 96 times a
 * frame. */
static uint8_t scale8(uint8_t v, uint8_t sc) {
  return (uint8_t)(((uint16_t)v * (uint16_t)((uint16_t)sc + 1u)) >> 8);
}

static uint16_t rngState = 12345;
static uint8_t random8(void) {
  uint8_t lsb = (uint8_t)(rngState & 1u);
  rngState >>= 1;
  if (lsb) rngState ^= 0xB400u;   /* taps for maximal length */
  return (uint8_t)rngState;
}

/* ======================================================================== *
 *  Data EEPROM  (128 bytes at 0x4000; we use 4)
 * ======================================================================== */
#define EEPROM_BASE            0x4000
#define EEPROM_MAGIC_ADDR      (EEPROM_BASE + 0)
#define EEPROM_COLOR_ADDR      (EEPROM_BASE + 1)
#define EEPROM_PATTERN_ADDR    (EEPROM_BASE + 2)
#define EEPROM_BRIGHTNESS_ADDR (EEPROM_BASE + 3)
#define EEPROM_MAGIC_VALUE     0xA5

static uint8_t currentColor      = 0;
static uint8_t currentPattern    = 0;
static uint8_t currentBrightness = DEFAULT_BRIGHTNESS;
static uint8_t isOn              = 0;
static uint8_t animStep          = 0;
static uint8_t breathDir         = 1;
static uint8_t breathVal         = 0;

static uint8_t eeprom_read(uint16_t addr) { return *((volatile uint8_t *)addr); }

static void eeprom_write(uint16_t addr, uint8_t data) {
  if (eeprom_read(addr) == data) return;        /* don't spend a cycle for nothing */
  FLASH->DUKR = 0xAE;                           /* unlock, RM0016 4.4.2 */
  FLASH->DUKR = 0x56;
  while (!(FLASH->IAPSR & FLASH_IAPSR_DUL));
  *((volatile uint8_t *)addr) = data;
  while (!(FLASH->IAPSR & FLASH_IAPSR_EOP));
  FLASH->IAPSR &= (uint8_t)~FLASH_IAPSR_DUL;    /* relock */
}

static void saveState(void) {
  eeprom_write(EEPROM_COLOR_ADDR, currentColor);
  eeprom_write(EEPROM_PATTERN_ADDR, currentPattern);
  eeprom_write(EEPROM_BRIGHTNESS_ADDR, currentBrightness);
  eeprom_write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
}

/* Invalidate saved state so a power cycle lands on defaults.  eeprom_write()
 * no-ops when the byte already reads 0x00, which is the common case at boot
 * and is why this is no longer a write per power-up. */
static void clearSavedState(void) { eeprom_write(EEPROM_MAGIC_ADDR, 0x00); }

static uint8_t loadState(void) {
  if (eeprom_read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VALUE) return 0;
  currentColor      = eeprom_read(EEPROM_COLOR_ADDR);
  currentPattern    = eeprom_read(EEPROM_PATTERN_ADDR);
  currentBrightness = eeprom_read(EEPROM_BRIGHTNESS_ADDR);
  if (currentColor   >= NUM_COLORS)       currentColor   = 0;
  if (currentPattern >= NUM_PATTERNS)     currentPattern = 0;
  if (currentBrightness < MIN_BRIGHTNESS) currentBrightness = DEFAULT_BRIGHTNESS;
  clearSavedState();          /* one restore per off->on, not unbounded */
  return 1;
}

/* ======================================================================== *
 *  UART1 debug on TP1 (PD5).  Transmit only -- RX would be PD6, the button.
 * ======================================================================== */
#if ENABLE_UART
#define UART_BAUD_DIV 139                    /* 16 MHz / 115200 = 138.9 */

static void uart_init(void) {
  UART1->CR1 = 0x00;
  UART1->CR3 = 0x00;                         /* 1 stop bit */
  /* BRR2 carries the low nibble and the top nibble of the divisor; BRR1 the
   * middle byte, and writing BRR1 is what latches the pair. */
  UART1->BRR2 = (uint8_t)((UART_BAUD_DIV & 0x0F) | ((UART_BAUD_DIV >> 8) & 0xF0));
  UART1->BRR1 = (uint8_t)((UART_BAUD_DIV >> 4) & 0xFF);
  UART1->CR2  = UART1_CR2_TEN;               /* UART1 drives PD5 from here */
}
static void uart_putc(char c) {
  while (!(UART1->SR & UART1_SR_TXE));
  UART1->DR = (uint8_t)c;
}
static void uart_puts(const char *s) { while (*s) uart_putc(*s++); }
static void uart_putu16(uint16_t v) {
  char buf[6];
  uint8_t i = 5;
  buf[5] = 0;
  if (!v) { uart_putc('0'); return; }
  while (v && i) { buf[--i] = (char)('0' + (uint8_t)(v % 10u)); v /= 10u; }
  uart_puts(&buf[i]);
}
static void uart_puthex8(uint8_t v) {
  static const char hx[] = "0123456789ABCDEF";
  uart_putc(hx[v >> 4]);
  uart_putc(hx[v & 0x0F]);
}
#else
#define uart_init()
#define uart_putc(c)
#define uart_puts(s)
#define uart_putu16(v)
#define uart_puthex8(v)
#endif

/* ======================================================================== *
 *  USB-C CC sense -> current budget
 *
 *  PD2/PD3 stay input-FLOATING (CR1 clear).  An internal pull-up corrupts
 *  the CC divider and misreads even a good 3 A source, so this is not a
 *  silent failure mode -- never blanket-init these two as pull-up inputs.
 * ======================================================================== */
static uint8_t ccTier      = 0;              /* 0 default, 1 = 1.5 A, 2 = 3.0 A */
static uint8_t ccMaxBright = BRIGHT_CAP_DEFAULT;

static void adc_init(void) {
  uint8_t d;
  gpio_in_float(CC_PORT, CC_MASK);
  /* SPSEL = 011 -> fADC = fmaster/6 = 2.67 MHz.  The datasheet ceiling is
   * 4 MHz below VDDA 4.5 V, and VDD here is whatever the USB port is doing. */
  ADC1->CR1 = 0x30;
  ADC1->CR2 = ADC1_CR2_ALIGN;                /* right aligned -> read DRL first */
  /* Disable the Schmitt trigger on AIN3/AIN4 to cut input leakage. */
  ADC1->TDRL |= (uint8_t)((1u << CC1_ADC_CH) | (1u << CC2_ADC_CH));
  ADC1->CR1 |= ADC1_CR1_ADON;                /* the first ADON only wakes it */
  for (d = 0; d < 60; d++) nop();            /* tSTAB ~ 7 us at 16 MHz */
}

static uint16_t adc_read(uint8_t ch) {
  uint8_t i;
  uint16_t v = 0;
  ADC1->CSR = ch;                            /* channel, EOC cleared, no IRQ */
  /* Two back-to-back conversions, keep the second.  Source impedance is
   * 10k + (5.1k || Rp) ~= 15.1k against CADC 3 pF + CAIN 3 pF: about 8 time
   * constants in the sample window, so one conversion is already good to
   * well under an LSB.  The repeat costs a few microseconds once a second
   * and makes the reading independent of whatever else ever uses the ADC. */
  for (i = 0; i < 2; i++) {
    ADC1->CR1 |= ADC1_CR1_ADON;              /* the second ADON starts it */
    while (!(ADC1->CSR & ADC1_CSR_EOC));
    v  = (uint16_t)ADC1->DRL;                /* DRL FIRST when right-aligned */
    v |= (uint16_t)((uint16_t)ADC1->DRH << 8);
    ADC1->CSR &= (uint8_t)~ADC1_CSR_EOC;
  }
  return v;
}

static void cc_poll(void) {
  uint16_t a  = adc_read(CC1_ADC_CH);
  uint16_t b  = adc_read(CC2_ADC_CH);
  uint16_t cc = (a > b) ? a : b;             /* mated orientation is the higher */
  if      (cc >= CC_TH_3A0) ccTier = 2;
  else if (cc >= CC_TH_1A5) ccTier = 1;
  else                      ccTier = 0;
  ccMaxBright = (ccTier == 2) ? BRIGHT_CAP_3A0
              : (ccTier == 1) ? BRIGHT_CAP_1A5
                              : BRIGHT_CAP_DEFAULT;
}

/* ======================================================================== *
 *  IR receiver on PA3 = TIM2_CH3 (J8, fitted DNP).  NEC protocol.
 *
 *  Decoded falling-edge to falling-edge with TIM2 at 8 us/tick: leader
 *  13.5 ms, '0' 1.125 ms, '1' 2.25 ms.  A NEC frame is the leader plus 32
 *  data-bit intervals; the trailing 560 us stop burst supplies the last
 *  edge.  showLEDs() masks interrupts for ~1.6 ms per 50 ms frame (32 LEDs;
 *  it scales with the strip) so an edge is occasionally lost -- NEC remotes
 *  repeat, so a dropped frame is not worth extra machinery.
 * ======================================================================== */
#if ENABLE_IR
#define IR_LEADER_MIN 1400
#define IR_LEADER_MAX 1900
#define IR_ZERO_MIN     90
#define IR_ZERO_MAX    190
#define IR_ONE_MIN     220
#define IR_ONE_MAX     350

static volatile uint32_t irShift = 0;
static volatile uint16_t irLast  = 0;
static volatile uint8_t  irBits  = 0;
static volatile uint32_t irFrame = 0;    /* addr, ~addr, cmd, ~cmd */
static volatile uint8_t  irReady = 0;

static void ir_init(void) {
  /* Pull-up so the pin does not float on the boards where J8 is not fitted. */
  gpio_in_pullup(IR_PORT, IR_MASK);
  TIM2->CCMR3 = 0x31;      /* CC3S = 01 (input on TI3) + input filter N = 8 */
  TIM2->CCER2 = 0x03;      /* CC3E | CC3P -> capture on the falling edge */
  TIM2->SR1   = (uint8_t)~TIM2_SR1_CC3IF;
  TIM2->IER  |= TIM2_IER_CC3IE;
}

INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14) {
  uint16_t now, d;
  uint8_t cmd, inv;
  if (!(TIM2->SR1 & TIM2_SR1_CC3IF)) return;
  now = (uint16_t)(((uint16_t)TIM2->CCR3H << 8) | TIM2->CCR3L);
  TIM2->SR1 = (uint8_t)~TIM2_SR1_CC3IF;
  d = (uint16_t)(now - irLast);          /* 16-bit wrap is intentional */
  irLast = now;
  if (d >= IR_LEADER_MIN && d <= IR_LEADER_MAX) {
    irShift = 0; irBits = 0;
  } else if (d >= IR_ZERO_MIN && d <= IR_ZERO_MAX) {
    irShift <<= 1; irBits++;
  } else if (d >= IR_ONE_MIN && d <= IR_ONE_MAX) {
    irShift = (irShift << 1) | 1UL; irBits++;
  } else {
    irBits = 0;                          /* out of spec: resync on next leader */
  }
  if (irBits >= 32) {
    cmd = (uint8_t)((irShift >> 8) & 0xFF);
    inv = (uint8_t)(irShift & 0xFF);
    if ((uint8_t)(~cmd) == inv) { irFrame = irShift; irReady = 1; }
    irBits = 0;
  }
}
#endif /* ENABLE_IR */

/* ======================================================================== *
 *  WS2812B driver -- bit-banged on PC6, NOP-counted at 16 MHz (1 NOP =
 *  62.5 ns).  '1' = 12 NOPs high / 4 low, '0' = 4 high / 12 low.  These are
 *  the timings running on shipped 150 mm units; do not retune them without
 *  a scope on J7 pin 2.
 * ======================================================================== */
#define NOP1  nop()
#define NOP2  NOP1; NOP1
#define NOP4  NOP2; NOP2
#define NOP10 NOP4; NOP4; NOP2

typedef struct { uint8_t r, g, b; } RGB;
static RGB ledBuffer[NUM_LEDS];

static void sendByte(uint8_t b) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    if (b & 0x80) {
      LED_PORT->ODR |= LED_MASK;
      NOP10; NOP2;
      LED_PORT->ODR &= (uint8_t)~LED_MASK;
      NOP4;
    } else {
      LED_PORT->ODR |= LED_MASK;
      NOP4;
      LED_PORT->ODR &= (uint8_t)~LED_MASK;
      NOP10; NOP2;
    }
    b <<= 1;
  }
}

/* Brightness -- including the source-current ceiling -- is applied here and
 * nowhere else, so no caller can route around the budget. */
static void showLEDs(void) {
  uint8_t i, br;
  volatile uint16_t j;

  br = currentBrightness;
  if (br > ccMaxBright) br = ccMaxBright;
  /* White auto-dim: solid white only.  Every other combination uses the
   * brightness verbatim.  See docs/pattern-dictionary.md 2.2. */
  if (currentColor == 0 && currentPattern == 0) br = scale8(br, WHITE_DIM_FACTOR);

  disableInterrupts();
  for (i = 0; i < NUM_LEDS; i++) {
    sendByte(scale8(ledBuffer[i].g, br));
    sendByte(scale8(ledBuffer[i].r, br));
    sendByte(scale8(ledBuffer[i].b, br));
  }
  enableInterrupts();
  /* Latch.  WS2812B wants >= 50 us; some clone strips want 280 us.  This
   * volatile loop is roughly 1.2 ms -- deliberately generous, inherited
   * from the shipping firmware, and outside the interrupts-off window, so
   * it costs nothing but frame duty. */
  for (j = 0; j < 2500; j++) nop();
  tb_poll();                            /* fold the masked window back in */
}

static void setAllLEDs(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t i;
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = r; ledBuffer[i].g = g; ledBuffer[i].b = b;
  }
}
static void clearLEDs(void) { setAllLEDs(0, 0, 0); showLEDs(); }

/* ======================================================================== *
 *  Palette and patterns -- canonical, see docs/pattern-dictionary.md
 * ======================================================================== */
static const RGB colors[NUM_COLORS] = {
  {255, 255, 255},  /* 0  White         */
  {255,   0,   0},  /* 1  Red           */
  {255,  64,   0},  /* 2  Red-Orange    */
  {255, 128,   0},  /* 3  Orange        */
  {255, 192,   0},  /* 4  Yellow-Orange */
  {255, 255,   0},  /* 5  Yellow        */
  {128, 255,   0},  /* 6  Yellow-Green  */
  {  0, 255,   0},  /* 7  Green         */
  {  0, 255, 128},  /* 8  Blue-Green    */
  {  0,   0, 255},  /* 9  Blue          */
  { 64,   0, 255},  /* 10 Blue-Purple   */
  {128,   0, 255},  /* 11 Purple        */
  {255,   0, 128}   /* 12 Red-Purple    */
};

typedef enum {
  PATTERN_SOLID = 0, PATTERN_RAINBOW, PATTERN_SCANNER, PATTERN_SPINNER,
  PATTERN_RANDOM, PATTERN_BREATHING, PATTERN_TWINKLE
} Pattern;

static void wheel(uint8_t pos, uint8_t *r, uint8_t *g, uint8_t *b) {
  if (pos < 85)       { *r = (uint8_t)(255 - pos * 3); *g = (uint8_t)(pos * 3); *b = 0; }
  else if (pos < 170) { pos -= 85;  *r = 0; *g = (uint8_t)(255 - pos * 3); *b = (uint8_t)(pos * 3); }
  else                { pos -= 170; *r = (uint8_t)(pos * 3); *g = 0; *b = (uint8_t)(255 - pos * 3); }
}

static void fadeAll(uint8_t amount) {
  uint8_t i;
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = ledBuffer[i].r > amount ? (uint8_t)(ledBuffer[i].r - amount) : 0;
    ledBuffer[i].g = ledBuffer[i].g > amount ? (uint8_t)(ledBuffer[i].g - amount) : 0;
    ledBuffer[i].b = ledBuffer[i].b > amount ? (uint8_t)(ledBuffer[i].b - amount) : 0;
  }
}

static void pattern_solid(void) {
  setAllLEDs(colors[currentColor].r, colors[currentColor].g, colors[currentColor].b);
}

static void pattern_rainbow(void) {
  uint8_t i, pos;
  for (i = 0; i < NUM_LEDS; i++) {
    pos = (uint8_t)(((uint16_t)i * 256u / NUM_LEDS + animStep) & 0xFFu);
    wheel(pos, &ledBuffer[i].r, &ledBuffer[i].g, &ledBuffer[i].b);
  }
  animStep += 3;
}

static void pattern_scanner(void) {
  uint8_t pos = (uint8_t)(animStep % (NUM_LEDS * 2));
  if (pos >= NUM_LEDS) pos = (uint8_t)((NUM_LEDS * 2) - pos - 1);
  fadeAll(20);
  ledBuffer[pos] = colors[currentColor];
  animStep++;
}

static void pattern_spinner(void) {
  /* Four equally spaced arms, each with a one-pixel tail. */
  uint8_t spacing, offset, arm, pos, tailPos;
  setAllLEDs(0, 0, 0);
  spacing = NUM_LEDS / 4;
  offset  = (uint8_t)(animStep % spacing);
  for (arm = 0; arm < 4; arm++) {
    pos = (uint8_t)(((uint16_t)arm * spacing + offset) % NUM_LEDS);
    ledBuffer[pos] = colors[currentColor];
    tailPos = (uint8_t)(((uint16_t)pos + NUM_LEDS - 1u) % NUM_LEDS);
    ledBuffer[tailPos].r = (uint8_t)(colors[currentColor].r / 3);
    ledBuffer[tailPos].g = (uint8_t)(colors[currentColor].g / 3);
    ledBuffer[tailPos].b = (uint8_t)(colors[currentColor].b / 3);
  }
  animStep++;
}

static void pattern_random(void) {
  fadeAll(15);
  if ((animStep % 3) == 0) ledBuffer[random8() % NUM_LEDS] = colors[currentColor];
  animStep++;
}

static void pattern_breathing(void) {
  uint8_t i;
  if (breathDir) { if (breathVal < 250) breathVal += 5; else breathDir = 0; }
  else           { if (breathVal >   5) breathVal -= 5; else breathDir = 1; }
  for (i = 0; i < NUM_LEDS; i++) {
    ledBuffer[i].r = scale8(colors[currentColor].r, breathVal);
    ledBuffer[i].g = scale8(colors[currentColor].g, breathVal);
    ledBuffer[i].b = scale8(colors[currentColor].b, breathVal);
  }
}

static void pattern_twinkle(void) {
  uint8_t i;
  for (i = 0; i < NUM_LEDS; i++) {
    if (random8() < 30) {
      ledBuffer[i] = colors[currentColor];
    } else {
      ledBuffer[i].r = ledBuffer[i].r > 10 ? (uint8_t)(ledBuffer[i].r - 10) : 0;
      ledBuffer[i].g = ledBuffer[i].g > 10 ? (uint8_t)(ledBuffer[i].g - 10) : 0;
      ledBuffer[i].b = ledBuffer[i].b > 10 ? (uint8_t)(ledBuffer[i].b - 10) : 0;
    }
  }
}

static void updatePattern(void) {
  switch (currentPattern) {
    case PATTERN_SOLID:     pattern_solid();     break;
    case PATTERN_RAINBOW:   pattern_rainbow();   break;
    case PATTERN_SCANNER:   pattern_scanner();   break;
    case PATTERN_SPINNER:   pattern_spinner();   break;
    case PATTERN_RANDOM:    pattern_random();    break;
    case PATTERN_BREATHING: pattern_breathing(); break;
    case PATTERN_TWINKLE:   pattern_twinkle();   break;
    default:                pattern_solid();     break;
  }
  showLEDs();
}

/* ======================================================================== *
 *  Button (SW1 on PD6, active low)
 *
 *  The gesture machine itself lives in button_logic.h as pure logic and is
 *  exercised on the host by tools/test_button_logic.c.  All that is left
 *  here is reading the pin.  It is driven by elapsed milliseconds rather
 *  than loop iterations, so the constants in config.h mean what they say
 *  and the machine does not care how long a frame took.
 * ======================================================================== */
#include "button_logic.h"

static ButtonSM btn_sm;

static uint8_t raw_pressed(void) { return (BTN_PORT->IDR & BTN_MASK) ? 0 : 1; }

/* ======================================================================== *
 *  Actions -- shared by the button and the remote so the two cannot drift.
 * ======================================================================== */
static void restartAnimation(void) { animStep = 0; breathVal = 0; breathDir = 1; }

static void act_power(void) {
  if (isOn) { saveState(); isOn = 0; clearLEDs(); }
  else      { isOn = 1; loadState(); restartAnimation(); }
}
static void act_next_color(void) {
  if (!isOn) return;
  if (++currentColor >= NUM_COLORS) currentColor = 0;
}
static void act_next_pattern(void) {
  if (!isOn) return;
  if (++currentPattern >= NUM_PATTERNS) currentPattern = 0;
  restartAnimation();
  setAllLEDs(0, 0, 0);            /* clean start for the new pattern */
}
static void act_bright_down(void) {
  if (!isOn) return;
  if (currentBrightness > (uint8_t)(MIN_BRIGHTNESS + BRIGHTNESS_STEP))
    currentBrightness -= BRIGHTNESS_STEP;
  else
    currentBrightness = DEFAULT_BRIGHTNESS;      /* wrap, per the UX contract */
}
static void act_bright_up(void) {
  if (!isOn) return;
  if (currentBrightness <= (uint8_t)(255 - BRIGHTNESS_STEP))
    currentBrightness += BRIGHTNESS_STEP;
  else
    currentBrightness = 255;
}

/* ======================================================================== *
 *  Bring-up mode -- no button, no remote, no EEPROM.
 *
 *  Flash with `make BRINGUP=1 flash` on a freshly assembled board.  It
 *  proves, in order: the MCU runs at all, PC6 reaches J7, the strip's
 *  colour order and length, and the CC tier the port advertises.  A wrong
 *  NUM_LEDS shows up immediately as dark tail pixels or a chase that wraps
 *  early.
 * ======================================================================== */
#if ENABLE_BRINGUP
static void bringup_forever(void) {
  uint8_t i;
  for (;;) {
    uart_puts("bringup: cc="); uart_putu16(ccTier);
    uart_puts(" cap=");        uart_putu16(ccMaxBright);
    uart_puts(" leds=");       uart_putu16(NUM_LEDS);
    uart_puts("\r\n");

    currentBrightness = 40;         /* deliberately dim: the supply is unknown */
    setAllLEDs(255, 0, 0);  showLEDs(); delay_ms(700);   /* expect RED   */
    setAllLEDs(0, 255, 0);  showLEDs(); delay_ms(700);   /* expect GREEN */
    setAllLEDs(0, 0, 255);  showLEDs(); delay_ms(700);   /* expect BLUE  */
    setAllLEDs(60, 60, 60); showLEDs(); delay_ms(700);

    /* One white pixel walking the whole strip: counts the LEDs for you. */
    for (i = 0; i < NUM_LEDS; i++) {
      setAllLEDs(0, 0, 0);
      ledBuffer[i].r = 255; ledBuffer[i].g = 255; ledBuffer[i].b = 255;
      showLEDs();
      delay_ms(60);
    }
    clearLEDs();
    delay_ms(500);
    cc_poll();
  }
}
#endif

/* ======================================================================== *
 *  main
 * ======================================================================== */
static void board_init(void) {
  CLK->CKDIVR = 0x00;              /* HSI/1, CPU/1 -> 16 MHz (reset is /8) */

  /* Data pin low before anything else, so the strip sees a clean idle. */
  gpio_out_pp_fast(LED_PORT, LED_MASK);
  LED_PORT->ODR &= (uint8_t)~LED_MASK;

  gpio_in_pullup(BTN_PORT, BTN_MASK);

  /* Tie off everything that would otherwise float.  PD1/SWIM and PD2/PD3
   * are excluded by construction -- see board.h. */
  gpio_in_pullup(GPIOA, IDLE_PU_PORTA);
  gpio_in_pullup(GPIOB, IDLE_PU_PORTB);
  gpio_in_pullup(GPIOC, IDLE_PU_PORTC);
  gpio_in_pullup(GPIOD, IDLE_PU_PORTD);

  button_reset(&btn_sm);
  timebase_init();
  adc_init();
  cc_poll();                       /* know the budget before the first frame */
  uart_init();
#if ENABLE_IR
  ir_init();                       /* this pulls PA3 up itself */
#else
  gpio_in_pullup(IR_PORT, IR_MASK);/* J8 is DNP: never leave PA3 floating */
#endif
  enableInterrupts();
}

void main(void) {
  uint16_t lastServiceMs, lastFrameMs, lastCcMs, nowMs;
  uint8_t  lastTier = 255;

  board_init();

  uart_puts("\r\nLL Basic rev2 | leds="); uart_putu16(NUM_LEDS);
  uart_puts(" caps=");  uart_putu16(BRIGHT_CAP_DEFAULT);
  uart_putc('/');       uart_putu16(BRIGHT_CAP_1A5);
  uart_putc('/');       uart_putu16(BRIGHT_CAP_3A0);
  uart_puts(" cc=");    uart_putu16(ccTier);
  uart_puts("\r\n");

  delay_ms(100);
  clearLEDs();
  delay_ms(50);

#if ENABLE_BRINGUP
  bringup_forever();               /* never returns */
#endif

  isOn = 0;
  /* A power cycle resets to defaults: users expect unplug-and-move to
   * forget.  Saved state is a convenience within one installation session. */
  clearSavedState();

  tb_poll();
  lastServiceMs = g_ms;
  lastFrameMs   = g_ms;
  lastCcMs      = g_ms;

  for (;;) {
    ButtonEvent btn;
    uint16_t dt;

    tb_poll();
    nowMs = g_ms;
    dt = (uint16_t)(nowMs - lastServiceMs);
    lastServiceMs = nowMs;

    /* ---- source budget ------------------------------------------------ */
    if ((uint16_t)(nowMs - lastCcMs) >= CC_POLL_MS) {
      lastCcMs = nowMs;
      cc_poll();
      if (ccTier != lastTier) {
        lastTier = ccTier;
        uart_puts("CC tier="); uart_putu16(ccTier);
        uart_puts(" cap=");    uart_putu16(ccMaxBright);
        uart_puts("\r\n");
      }
    }

    /* ---- remote ------------------------------------------------------- */
#if ENABLE_IR
    if (irReady) {
      uint32_t f;
      uint8_t c;
      disableInterrupts();
      f = irFrame;
      irReady = 0;
      enableInterrupts();
      c = (uint8_t)((f >> 8) & 0xFFu);
      /* Unknown codes are printed, not ignored -- that is how you learn a
       * remote.  Address and command, both in hex. */
      uart_puts("IR addr=0x"); uart_puthex8((uint8_t)((f >> 24) & 0xFFu));
      uart_puts(" cmd=0x");    uart_puthex8(c);
      uart_puts("\r\n");
      switch (c) {
        case IR_CMD_POWER:     act_power();        break;
        case IR_CMD_COLOR:     act_next_color();   break;
        case IR_CMD_PATTERN:   act_next_pattern(); break;
        case IR_CMD_BRIGHT_UP: act_bright_up();    break;
        case IR_CMD_BRIGHT_DN: act_bright_down();  break;
        default: break;
      }
    }
#endif

    /* ---- button ------------------------------------------------------- */
    btn = button_service(&btn_sm, dt, raw_pressed());
    switch (btn) {
      case BTN_SINGLE: if (!isOn) act_power(); else act_next_color(); break;
      case BTN_DOUBLE: act_next_pattern();  break;
      case BTN_TRIPLE: act_bright_down();   break;
      case BTN_LONG:   if (isOn) act_power(); break;
      default: break;
    }

    /* ---- frame -------------------------------------------------------- */
    if ((uint16_t)(nowMs - lastFrameMs) >= PATTERN_STEP_MS) {
      lastFrameMs = nowMs;
      if (isOn) updatePattern();
    }
  }
}
