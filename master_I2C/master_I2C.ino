/*
 * Teensy 4.0 I2C Master - Read Encoder Counter from ATTiny202
 * 
 * Connections:
 * - Teensy Pin 18 (SDA) -> ATTiny202 physical pin 5 (PA2/SDA) -> 5.1kΩ -> 3.3V
 * - Teensy Pin 19 (SCL) -> ATTiny202 physical pin 4 (PA1/SCL) -> 5.1kΩ -> 3.3V
 * - GND -> GND
 * - 3.3V -> VCC (ATTiny202)
 * 
 * NOTE: Use 3.3V pull-ups, not 5V (Teensy 4.0 is 3.3V logic)
 */

#include <Wire.h>

#define SLAVE_ADDRESS  0x08  // Testing different address

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Use default I2C pins (18, 19) on Teensy 4.0
  Wire.begin();
  
  Serial.println("I2C Master initialized");
  Serial.println("Looking for encoder at address 0x08...");
}

void loop() {
  Serial.print("Sending address 0x");
  Serial.print(SLAVE_ADDRESS, HEX);
  Serial.print("... ");
  
  Wire.beginTransmission(SLAVE_ADDRESS);
  uint8_t error = Wire.endTransmission();
  
  // Error codes: 0=success, 1=data too long, 2=NACK on address, 3=NACK on data, 4=other, 5=timeout
  Serial.print("Error: ");
  Serial.print(error);
  if (error == 2) Serial.println(" (NACK - slave not acknowledging address)");
  else if (error == 0) Serial.println(" (ACK - slave responded!)");
  else Serial.println();
  
  if (error == 0) {
    Serial.println("Requesting 4 bytes...");
    Wire.requestFrom(SLAVE_ADDRESS, 4);
    Serial.print("Available bytes: ");
    Serial.println(Wire.available());
    
    if (Wire.available() >= 4) {
      int32_t counter = 0;
      Wire.readBytes((uint8_t*)&counter, 4);
      Serial.print("Counter: ");
      Serial.println(counter);
    } else {
      Serial.println("Not enough bytes received");
    }
  }
  
  Serial.println("---");
  delay(1000);
}
 