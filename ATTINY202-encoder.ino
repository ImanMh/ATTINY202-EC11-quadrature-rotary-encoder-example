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
// #define I2C_SCL_PIN    2    // I2C SCL pin (PA1, physical pin 4)
// #define I2C_SDA_PIN    3    // I2C SDA pin (PA2, physical pin 5)

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
// I2C Request Handler
// ============================================================================
void requestEvent() {
  // Send encoder counter as 4 bytes (int32_t, little-endian)
  // Read atomically by copying to local variable (32-bit read on 8-bit CPU needs protection)
  int32_t counterCopy;
  noInterrupts();
  counterCopy = encoderCounter;
  interrupts();
  
  uint8_t* counterPtr = (uint8_t*)&counterCopy;
  Wire.write(counterPtr, 4);
}

// ============================================================================
// Setup Function
// ============================================================================
void setup() 
{
  // Initialize I2C as slave with explicit pin configuration
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);  // Register callback for I2C read requests
  
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

  // Update encoder counter (atomic operation - delta() already handles synchronization)
  if (delta != 0) {
    // Update counter atomically by disabling interrupts briefly
    noInterrupts();
    encoderCounter += delta;
    interrupts();
  }
  
  // Main loop can do other tasks here if needed
  // The encoder counter is updated in the interrupt context via encoder.delta()
  // and exposed via I2C in the requestEvent() callback
}