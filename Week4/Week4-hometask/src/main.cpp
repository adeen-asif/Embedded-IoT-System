#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED setup ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // If display not found, stop program
    for (;;);
  }

  display.clearDisplay();

  // ---- Display name and ID ----
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(15, 20);          // Move text slightly right and down
  display.println("Name: Adeen Asif");

  display.setCursor(15, 35);          // Next line for ID
  display.println("ID: 23-NTU-CS-1007");

  // ---- Draw box around text ----
  display.drawRect(10, 15, 108, 30, SSD1306_WHITE);
  // (x=10, y=15, width=108, height=30)

  display.display();  // Show everything on the screen
}

void loop() {
  // Nothing needed in loop (static display)
}
