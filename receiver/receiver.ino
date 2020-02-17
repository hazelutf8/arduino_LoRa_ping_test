#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // https://github.com/adafruit/Adafruit_SSD1306

#include <SPI.h>
#include <LoRa.h> // https://github.com/sandeepmistry/arduino-LoRa
uint8_t val = 0;
#define BUFSIZE 256
char buf[BUFSIZE];

#define BUZZER 2

#define SCREEN_WIDTH 128
#define SCREEN_HIGHT  32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HIGHT, &Wire, OLED_RESET);

/**
 * @brief Init OLED, Serial and LoRa
 */
void setup() {
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  Serial.begin(9600);
  
  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6)) { // or 915E6
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.dumpRegisters(Serial);
}


/**
 * @brief Poll on available LoRa packet bytes, and alert on received packet.
 */
void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) { 
    // read packet
    uint32_t read_len = 0;
    while (LoRa.available())
    {
      const int readChar = LoRa.read();
      buf[read_len] = (char)(uint8_t)readChar;
      read_len++;
    }
    buf[read_len] = '\0';
	  tone(BUZZER, 1000, 100);
    PrintHex8(buf, read_len);
  }
}


/**
 * @brief Print data payload buffer in hex and append RSSI information
 */
void PrintHex8(uint8_t *data, uint8_t length)
{
     char tmp[length*2+1];
     byte first;
     byte second;
     for (int i=0; i<length; i++) {
           first = (data[i] >> 4) & 0x0f;
           second = data[i] & 0x0f;
           // base for converting single digit numbers to ASCII is 48
           // base for 10-16 to become lower-case characters a-f is 87
           // note: difference is 39
           tmp[i*2] = first+48;
           tmp[i*2+1] = second+48;
           if (first > 9) tmp[i*2] += 39;
           if (second > 9) tmp[i*2+1] += 39;
     }
     tmp[length*2] = 0; // Ensure it is NULL terminated at the end.
     {// Print to serial
       Serial.print("0x");
       Serial.print(tmp);
       Serial.print(" ");
       Serial.println(LoRa.packetRssi());
     }
     { // Print to display
       display.clearDisplay();
       display.setTextSize(2);
       display.setTextColor(WHITE);
       display.setCursor(0,0);
       display.print("0x");
       display.print(tmp);
       display.print(" ");
       display.println(LoRa.packetRssi());
       display.display();
    }
}
