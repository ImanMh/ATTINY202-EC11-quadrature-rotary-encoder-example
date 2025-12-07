/*
 * ATTiny202 Rotary Encoder with LED Feedback
 * 
 * This sketch uses the John-Lluch Encoder library to read a rotary encoder
 * and provides visual feedback via LEDs for clockwise and counter-clockwise rotation.
 * 
 * Hardware:
 * - Encoder channel A connected to pin 1 (CH_A)
 * - Encoder channel B connected to pin 2 (CH_B)
 * - LED for clockwise rotation on pin 4 (LED_PIN_CW)
 * - LED for counter-clockwise rotation on pin 0 (LED_PIN_CCW)
 * 
 * The encoder is configured as active-low with internal pull-ups enabled.
 */

#include "Encoder.h"

// ============================================================================
// Hardware Pin Definitions
// ============================================================================
#define CH_A           1    // Encoder channel A pin
#define CH_B           2    // Encoder channel B pin
#define CH_P           255  // Pushbutton pin (not used, set to invalid pin)
#define LED_PIN_CW     4    // LED for clockwise rotation
#define LED_PIN_CCW    0    // LED for counter-clockwise rotation

// ============================================================================
// Configuration Constants
// ============================================================================
#define INTERRUPT_PERIOD       25   // Timer interrupt period (25 = 10kHz, 0.1ms sampling)
#define LED_BLINK_DURATION_MS  100  // LED blink duration in milliseconds

// ============================================================================
// Global Variables
// ============================================================================
Encoder encoder(CH_A, CH_B, CH_P);

// LED control counters (decremented each millisecond)
volatile uint16_t ledCWCounter = 0;
volatile uint16_t ledCCWCounter = 0;

// ============================================================================
// Setup Function
// ============================================================================
void setup() 
{
  // Initialize LED pins as outputs
  pinMode(LED_PIN_CW, OUTPUT);
  pinMode(LED_PIN_CCW, OUTPUT);
  digitalWrite(LED_PIN_CW, LOW);
  digitalWrite(LED_PIN_CCW, LOW);
  
  // Initialize encoder with timer-based interrupts
  // This sets up a 10kHz timer interrupt to poll the encoder state
  EncoderInterrupt.begin(&encoder);
}

// ============================================================================
// Main Loop
// ============================================================================
void loop()
{
  // Read encoder movement since last call
  // Returns positive for clockwise, negative for counter-clockwise, 0 if no movement
  int delta = encoder.delta();

  // Update LED blink counters based on encoder movement
  if (delta != 0) {
    if (delta > 0) {
      // Clockwise rotation detected
      ledCWCounter = LED_BLINK_DURATION_MS;
    } else {
      // Counter-clockwise rotation detected
      ledCCWCounter = LED_BLINK_DURATION_MS;
    }
  }

  // Update LED states every millisecond
  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = millis();
  
  if (currentTime != lastUpdateTime) {
    lastUpdateTime = currentTime;
    
    // Update clockwise LED
    if (ledCWCounter > 0) {
      digitalWrite(LED_PIN_CW, HIGH);
      ledCWCounter--;
    } else {
      digitalWrite(LED_PIN_CW, LOW);
    }
    
    // Update counter-clockwise LED
    if (ledCCWCounter > 0) {
      digitalWrite(LED_PIN_CCW, HIGH);
      ledCCWCounter--;
    } else {
      digitalWrite(LED_PIN_CCW, LOW);
    }
  }
}