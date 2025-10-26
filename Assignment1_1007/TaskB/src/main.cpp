// Task 2  (Assignment 1 )
// Adeen Asif
// 23-NTU-CS-1007

// include necessary libraries
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup 
// Define the width and height of the OLED screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pin configuration 
const int buttonPin = 25;
const int ledPin = 5; // red led
const int buzzerPin = 33;

unsigned long buttonPressTime = 0;
bool buttonPressed = false;
bool ledState = false;  // current led state

// Timing thresholds 
const unsigned long longPressTime = 1500; // 1.5 seconds

// display events
void showEvent(const char* event) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Event: ");
  display.println(event);
  display.display();
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // led OUTPUT 
  pinMode(buzzerPin, OUTPUT); // buzzer INPUT

  // OLED initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  // first message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("System Ready");
  display.display();
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && !buttonPressed) {
    // Button just pressed
    buttonPressed = true;
    buttonPressTime = millis();
  }

  if (buttonState == HIGH && buttonPressed) {
    // Button released
    buttonPressed = false;
    unsigned long pressDuration = millis() - buttonPressTime;

    if (pressDuration < longPressTime) {
      // Short press (toggle LED)
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);
      showEvent("Short Press: LED Toggled");
    } else {
      // Long press (play buzzer)
      tone(buzzerPin, 1000, 500); // 1 kHz tone for 0.5s
      showEvent("Long Press: Buzzer Played"); // show event
    }
  }
}
