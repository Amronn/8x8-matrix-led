#include <Arduino.h>
#include <SPI.h>

SPIClass *spi = nullptr;
const static int spiClk = 10000;

const uint8_t num_of_matrices = 4;

void displayPixel(byte x, byte y) {
  if (x > 31 || y > 7) {
    return;
  }
  byte index = x / 8;
  byte address = 1 << (x % 8);
  byte command = 0x01 + y;

  digitalWrite(4, LOW);
  for (byte i = 0; i < num_of_matrices; i++) {
        if (i == 3 - index) {
            spi->transfer(command);
            spi->transfer(address);
        } else {
            spi->transfer(0x00);
            spi->transfer(0x00);
        }
    }
  digitalWrite(4, HIGH);
}
void clearDisplay() {
  for (byte i = 0; i < 8; i++) {
      digitalWrite(4, LOW);
      for (byte j = 0; j < 4; j++) {
          spi->transfer(0x01 + i);
          spi->transfer(0x00);
      }
      digitalWrite(4, HIGH);
  }
}
void setup() {
  spi = new SPIClass(SPI);
  spi->begin(12, 11, 13); // SCK=13, MOSI=11, MISO=12
  spi->setHwCs(false);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  digitalWrite(4, LOW);
  spi->transfer(0x0C); // Shutdown register
  spi->transfer(0x01); // Normal operation
  digitalWrite(4, HIGH);

  digitalWrite(4, LOW);
  spi->transfer(0x0F); // Display test register
  spi->transfer(0x00); // Disable test mode
  digitalWrite(4, HIGH);

  digitalWrite(4, LOW);
  spi->transfer(0x09); // Decode mode register
  spi->transfer(0x00); // No decode for all digits
  digitalWrite(4, HIGH);

  digitalWrite(4, LOW);
  spi->transfer(0x0A); // Intensity register
  spi->transfer(0x00); // Medium brightness
  digitalWrite(4, HIGH);

  digitalWrite(4, LOW);
  spi->transfer(0x0B); // Scan limit register
  spi->transfer(0x07); // Display all digits
  digitalWrite(4, HIGH);

  for(int i = 0; i < 4; i++) {
    digitalWrite(4, LOW);
    spi->transfer(0x00); // Scan limit register
    spi->transfer(0x00); // Display all digits
    digitalWrite(4, HIGH);
  }

  clearDisplay();
  // displayPixel(2, 3);
}

void loop() {
  for (int i = 0; i < 32; i++) {
    displayPixel(i, 0);
    delay(100);
    clearDisplay();
  }

}
