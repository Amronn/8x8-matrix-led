#include <Arduino.h>
#include <SPI.h>

SPIClass *spi = nullptr;
const static int spiClk = 10000;

const uint8_t num_of_matrices = 4;

const uint8_t smiley_face[8] = {
  0b00111100,
  0b01000010,
  0b01000010,
  0b00011000,
  0b00000000,
  0b01011010,
  0b00111100,
  0b00000000
};

void drawSmileyFace() {
  for (byte i = 0; i < 8; i++) {
    digitalWrite(4, LOW);
    for (byte j = 0; j < num_of_matrices; j++) {
      spi->transfer(0x01 + i); // Address
      spi->transfer(smiley_face[i]); // Data
    }
    digitalWrite(4, HIGH);
  }
}
void drawLine(byte x1, byte y1, byte x2, byte y2) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    displayPixel(x1, y1);
    if (x1 == x2 && y1 == y2) break;
    int err2 = err * 2;
    if (err2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (err2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}
void drawRectangle(byte x, byte y, byte width, byte height) {
  drawLine(x, y, x + width - 1, y);
  drawLine(x + width - 1, y, x + width - 1, y + height - 1);
  drawLine(x + width - 1, y + height - 1, x, y + height - 1);
  drawLine(x, y + height - 1, x, y);
}
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
