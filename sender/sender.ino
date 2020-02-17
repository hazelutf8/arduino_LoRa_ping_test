#include <SPI.h>
#include <LoRa.h> // https://github.com/sandeepmistry/arduino-LoRa

uint8_t COUNT;


/*
 * @brief Setup serial and LoRa radio
 */
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  COUNT = 0;
  LoRa.dumpRegisters(Serial);
}


/**
 * @brief Increment and transmit count byte over LoRa
 */
void loop() {
  LoRa.beginPacket();  
  LoRa.write((uint8_t)COUNT);
  LoRa.endPacket();
  
  uint8_t version = LoRa.readRegister(0x42);
  Serial.print(version);
  Serial.print(" ");
  Serial.println(COUNT);
  
  COUNT += 1;
  delay(1000);
}
