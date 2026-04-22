#include "stm8s.h"

// =================== Hardware ===================
#define NUM_LEDS 32

// LED data on PC7 (custom PCB, TSSOP20 pin 17)
#define DATA_PORT GPIOC
#define DATA_MASK GPIO_PIN_7

// Button on PB5
#define BTN_PORT GPIOD
#define BTN_MASK GPIO_PIN_6

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
