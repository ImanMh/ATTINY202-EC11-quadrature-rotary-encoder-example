/*
 * Minimal Teensy I2C Master - Read Encoder Counter from ATTiny202
 * 
 * Connections:
 * - Teensy Pin 11 (SDA) -> ATTiny202 PA2 (pin 3, physical pin 5) -> 5.1kΩ -> VCC
 * - Teensy Pin 10 (SCL) -> ATTiny202 PA1 (pin 2, physical pin 4) -> 5.1kΩ -> VCC
 * - GND -> GND
 * - 5V -> VCC
 */

#include <Wire.h>

#define SLAVE_ADDRESS  0x08
// ATTiny202: PA1=SCL (pin 2, physical pin 4), PA2=SDA (pin 3, physical pin 5)
// You connected: PA1->Teensy10, PA2->Teensy11
#define SDA_PIN        11    // Teensy pin 11 (ATTiny202 PA2/SDA is pin 3, physical pin 5)
#define SCL_PIN        10    // Teensy pin 10 (ATTiny202 PA1/SCL is pin 2, physical pin 4)

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
  
  Serial.println("I2C Scanner - Looking for devices...");
  scanI2C();
  
  Serial.println("\nStarting encoder counter reader...");
  Serial.println();
}

void loop() {
  // First check if slave responds to address
  Wire.beginTransmission(SLAVE_ADDRESS);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    // Slave acknowledged - request data
    Wire.requestFrom(SLAVE_ADDRESS, 4);
    
    if (Wire.available() >= 4) {
      int32_t counter = 0;
      Wire.readBytes((uint8_t*)&counter, 4);
      Serial.print("Counter: ");
      Serial.println(counter);
    } else {
      Serial.print("Only ");
      Serial.print(Wire.available());
      Serial.println(" bytes available");
      while (Wire.available()) Wire.read();
    }
  } else {
    Serial.print("Slave not responding (error: ");
    Serial.print(error);
    Serial.println(")");
  }
  
  delay(500);
}

void scanI2C() {
  uint8_t found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      if (addr == SLAVE_ADDRESS) {
        Serial.print(" <-- This is our encoder!");
      }
      Serial.println();
      found++;
    }
  }
  
  if (found == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("Check:");
    Serial.println("  1. Pull-up resistors (5.1kΩ) on SDA and SCL");
    Serial.println("  2. Wiring connections");
    Serial.println("  3. Power to ATTiny202");
  }
}
 
 