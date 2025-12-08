/*
 * ATTiny202 Rotary Encoder with I2C Interface
 * 
 * This sketch uses the John-Lluch Encoder library to read a rotary encoder
 * and exposes the encoder position counter via I2C.
 * 
 * Hardware (megaTinyCore pin numbers):
 * - Encoder channel A connected to pin 0 (CH_A) = PA6 (physical pin 2)
 * - Encoder channel B connected to pin 1 (CH_B) = PA7 (physical pin 3)
 * - I2C SDA on pin 3 (PA2, physical pin 5)
 * - I2C SCL on pin 2 (PA1, physical pin 4)
 * 
 * Pin mapping (pin number → Port pin → Physical pin):
 *   0 → PA6 → 2
 *   1 → PA7 → 3
 *   2 → PA1 → 4  (I2C SCL)
 *   3 → PA2 → 5  (I2C SDA)
 *   4 → PA3 → 7
 * 
 * I2C Configuration:
 * - Slave address: 0x08
 * - Responds to I2C read requests with 4-byte int32_t encoder counter value
 * 
 * The encoder is configured as active-low with internal pull-ups enabled.
 */

#include "Encoder.h"
#include <Wire.h>

// ============================================================================
// Hardware Pin Definitions
// ============================================================================
#define CH_A           0    // Encoder channel A pin (PA6, physical pin 2)
#define CH_B           1    // Encoder channel B pin (PA7, physical pin 3)
#define CH_P           255  // Pushbutton pin (not used, set to invalid pin)
#define DEBUG_PIN      4    // Debug pin (PA3, physical pin 7)

// ============================================================================
// Configuration Constants
// ============================================================================
#define INTERRUPT_PERIOD       25   // Timer interrupt period (25 = 10kHz, 0.1ms sampling)
#define I2C_SLAVE_ADDRESS      0x08 // I2C slave address

// ============================================================================
// Global Variables
// ============================================================================
Encoder encoder(CH_A, CH_B, CH_P);

// Encoder position counter (accumulated from encoder deltas)
// Must be volatile since it's read from interrupt context (I2C requestEvent)
volatile int32_t encoderCounter = 0;

// ============================================================================
// I2C Event Handlers
// ============================================================================
void receiveEvent(int numBytes) {
  // DEBUG: Toggle to verify ANY I2C traffic is received
  static bool toggle = false;
  digitalWrite(DEBUG_PIN, toggle);
  toggle = !toggle;
  
  while(Wire.available()) Wire.read();
}

void requestEvent() {
  // DEBUG: Toggle to verify requestEvent is called
  static bool toggle = false;
  digitalWrite(DEBUG_PIN, toggle);
  toggle = !toggle;
  
  int32_t counterCopy;
  noInterrupts();
  counterCopy = encoderCounter;
  interrupts();
  Wire.write((uint8_t*)&counterCopy, 4);
}

// ============================================================================
// Setup Function
// ============================================================================
void setup() 
{
  pinMode(DEBUG_PIN, OUTPUT);
  digitalWrite(DEBUG_PIN, LOW);
  
  // TEST: Initialize I2C WITHOUT encoder to check for conflicts
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  // Temporarily disable encoder to test I2C alone
  // EncoderInterrupt.begin(&encoder);
}

// ============================================================================
// Main Loop
// ============================================================================
void loop()
{
  // Temporarily disabled for I2C testing
  // int delta = encoder.delta();
  // if (delta != 0) {
  //   noInterrupts();
  //   encoderCounter += delta;
  //   interrupts();
  // }
}