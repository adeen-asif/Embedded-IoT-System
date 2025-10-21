#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Pin configuration ---
#define LDR_PIN 34       // LDR analog pin
#define DHTPIN 14        // DHT22 data pin
#define DHTTYPE DHT22    // Change to DHT11 if needed
#define SDA_PIN 21       // I2C SDA
#define SCL_PIN 22       // I2C SCL

// --- OLED setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DHT sensor setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Setup function ---
void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Initialize DHT
  dht.begin();
  delay(1000);
}

// --- Main loop ---
void loop() {
  // --- Read sensors ---
  int adcValue = analogRead(LDR_PIN);
  float voltage = (adcValue / 4095.0) * 3.3;
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if DHT read failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT22 sensor!");
    return;
  }

  // --- Serial Output ---
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(humidity);
  Serial.print(" %  |  LDR ADC: ");
  Serial.print(adcValue);
  Serial.print("  |  Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  // --- OLED Display (Order: Temp → Humidity → LDR) ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Hello IoT");

  display.setCursor(0, 12);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.setCursor(0, 26);
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  display.setCursor(0, 40);
  display.print("LDR: ");
  display.print(adcValue);
  display.print("  ");
  display.print(voltage, 2);
  display.println("V");

  display.display();

  delay(2000); // update every 2 seconds
}
