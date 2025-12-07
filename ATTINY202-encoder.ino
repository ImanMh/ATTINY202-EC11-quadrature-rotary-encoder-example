#include "Encoder.h"

// Pin definitions from our hardware setup
#define LED_PIN_CCW 0
#define LED_PIN_CW 4
#define CH_A 1
#define CH_B 2
#define CH_P 255  // No pushbutton, use invalid pin number

// create an encoder object initialized with their pins
Encoder encoder( CH_A, CH_B, CH_P );

// LED control
volatile uint16_t ledCWCounter = 0;
volatile uint16_t ledCCWCounter = 0;
#define LED_BLINK_DURATION 100  // milliseconds

// setup code
void setup() 
{
  // Configure LED pins as outputs
  pinMode(LED_PIN_CW, OUTPUT);
  pinMode(LED_PIN_CCW, OUTPUT);
  digitalWrite(LED_PIN_CW, LOW);
  digitalWrite(LED_PIN_CCW, LOW);
  
  // start the encoder time interrupts
  EncoderInterrupt.begin( &encoder );
}

// loop code
void loop()
{
  // get the encoder variation since our last check, it can be positive or negative, or zero if the encoder didn't move
  // only call this once per loop cycle, or at any time you want to know any incremental change
  int delta = encoder.delta();

  // Process encoder movement for LED control
  if (delta != 0) {
    if (delta > 0) {
      // Clockwise rotation
      ledCWCounter = LED_BLINK_DURATION;
    } else {
      // Counter-clockwise rotation
      ledCCWCounter = LED_BLINK_DURATION;
    }
  }

  // Update LED blink counters and control
  static uint32_t lastUpdate = 0;
  uint32_t currentMillis = millis();
  
  // Update every millisecond
  if (currentMillis != lastUpdate) {
    lastUpdate = currentMillis;
    
    // Update LED blink counters and control
    if (ledCWCounter > 0) {
      digitalWrite(LED_PIN_CW, HIGH);
      ledCWCounter--;
    } else {
      digitalWrite(LED_PIN_CW, LOW);
    }
    
    if (ledCCWCounter > 0) {
      digitalWrite(LED_PIN_CCW, HIGH);
      ledCCWCounter--;
    } else {
      digitalWrite(LED_PIN_CCW, LOW);
    }
  }
}