#define LED_PIN_CCW 0
#define LED_PIN_CW 4
#define CH_A 1
#define CH_B 2

// Encoder state tracking
volatile uint8_t encoderState = 0;
volatile uint8_t lastEncoderState = 0;
volatile int8_t encoderPosition = 0;

// LED control
volatile uint8_t ledCWState = 0;
volatile uint8_t ledCCWState = 0;
volatile uint16_t ledCWCounter = 0;
volatile uint16_t ledCCWCounter = 0;
#define LED_BLINK_DURATION 100  // milliseconds

void setup() {
  // Configure LED pins as outputs
  pinMode(LED_PIN_CW, OUTPUT);
  pinMode(LED_PIN_CCW, OUTPUT);
  digitalWrite(LED_PIN_CW, LOW);
  digitalWrite(LED_PIN_CCW, LOW);
  
  // Configure encoder pins as inputs with pull-ups
  pinMode(CH_A, INPUT_PULLUP);
  pinMode(CH_B, INPUT_PULLUP);
  
  // Read initial encoder state
  lastEncoderState = (digitalRead(CH_A) << 1) | digitalRead(CH_B);
  encoderState = lastEncoderState;
  
  // Enable pin change interrupts on encoder pins
  // ATTiny202 uses PORTMUX and ISC settings
  // For pins 1 and 2, we need to configure PCINT
  // Active low encoder: detect rising edges as start of state changes
  PORTA.PIN1CTRL |= PORT_ISC_RISING_gc;  // Pin change interrupt on rising edge for CH_A
  PORTA.PIN2CTRL |= PORT_ISC_RISING_gc;  // Pin change interrupt on rising edge for CH_B
  
  // Enable interrupts globally
  sei();
}

// Quadrature decoder lookup table
// Index: (lastState << 2) | currentState
// Value: -1 = CCW, 0 = invalid/no change, +1 = CW
// Gray code transitions: 00->01(+1), 01->11(+1), 11->10(+1), 10->00(+1)
//                       00->10(-1), 10->11(-1), 11->01(-1), 01->00(-1)
const int8_t quadratureTable[16] = {
  0,   // 00->00: no change
  +1,  // 00->01: CW
  -1,  // 00->10: CCW
  0,   // 00->11: invalid (both changed)
  -1,  // 01->00: CCW
  0,   // 01->01: no change
  0,   // 01->10: invalid (both changed)
  +1,  // 01->11: CW
  +1,  // 10->00: CW
  0,   // 10->01: invalid (both changed)
  0,   // 10->10: no change
  -1,  // 10->11: CCW
  0,   // 11->00: invalid (both changed)
  -1,  // 11->01: CCW
  +1,  // 11->10: CW
  0    // 11->11: no change
};

// Pin change interrupt service routine
ISR(PORTA_PORT_vect) {
  // Read current encoder state
  // Using digitalRead for Arduino compatibility (can be optimized to direct register access if needed)
  uint8_t a = digitalRead(CH_A);
  uint8_t b = digitalRead(CH_B);
  encoderState = (a << 1) | b;
  
  // Look up direction from transition table
  uint8_t transition = (lastEncoderState << 2) | encoderState;
  int8_t direction = quadratureTable[transition];
  
  if (direction != 0) {
    encoderPosition += direction;
    
    if (direction > 0) {
      // Clockwise rotation
      ledCWState = 1;
      ledCWCounter = LED_BLINK_DURATION;
    } else {
      // Counter-clockwise rotation
      ledCCWState = 1;
      ledCCWCounter = LED_BLINK_DURATION;
    }
    
    lastEncoderState = encoderState;
  }
  
  // Clear interrupt flags
  PORTA.INTFLAGS = 0xFF;
}

void loop() {
  uint32_t currentMillis = millis();
  static uint32_t lastUpdate = 0;
  
  // Update every millisecond
  if (currentMillis != lastUpdate) {
    lastUpdate = currentMillis;
    
    // Update LED blink counters and control
    if (ledCWCounter > 0) {
      ledCWCounter--;
      digitalWrite(LED_PIN_CW, HIGH);
    } else {
      digitalWrite(LED_PIN_CW, LOW);
      ledCWState = 0;
    }
    
    if (ledCCWCounter > 0) {
      ledCCWCounter--;
      digitalWrite(LED_PIN_CCW, HIGH);
    } else {
      digitalWrite(LED_PIN_CCW, LOW);
      ledCCWState = 0;
    }
  }
}
