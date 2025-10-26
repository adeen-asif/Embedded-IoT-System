// Task 1 (Assignment 1)
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
const int btnMode = 14;  // button to switch modes
const int btnReset = 33;// reset button 
const int led1 = 26;   // yellow led
const int led2 = 4;   // blue led
const int led3 = 5;  // red led

int mode = 0; // 0=OFF, 1=Alt Blink, 2=Both ON, 3=PWM Fade
unsigned long lastToggle = 0;
bool ledState = false;

// OLED display update 
void showMode() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Mode: ");
  switch (mode) {
    case 0: display.print("Both OFF"); break;
    case 1: display.print("Alternate Blink"); break;
    case 2: display.print("Both ON"); break;
    case 3: display.print("PWM Fade"); break;
  }
  display.display();
}

void setup() {
  // pin setup
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(btnMode, INPUT_PULLUP);
  pinMode(btnReset, INPUT_PULLUP);

  // OLED initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  // first message display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("System Ready"); // OLED display
  display.display();
  delay(1000); // display message for 1 sec
  showMode(); // show current mode
}

void loop() {
  static int lastBtnState1 = HIGH; // last button state
  static int lastBtnState2 = HIGH; // for reset button

  int btn1 = digitalRead(btnMode);  // read button mode
  int btn2 = digitalRead(btnReset); // read reset button

  // Button 1 (cycle through LED modes)
  if (btn1 == LOW && lastBtnState1 == HIGH) {
    mode++; // next mode
    if (mode > 3) mode = 0;
    showMode(); // show on display
    delay(200); // debounce
  }

  // Button 2 (reset to OFF)
  if (btn2 == LOW && lastBtnState2 == HIGH) {
    mode = 0;   // turn everything OFF
    showMode(); // show on display
    delay(200); // debounce
  }

  lastBtnState1 = btn1;
  lastBtnState2 = btn2;

  // LED behavior 
  switch (mode) {
    case 0: // Both OFF
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      break;

    case 1: // Alternate blink
      if (millis() - lastToggle >= 500) // toggle every 500ms
      {
    
        lastToggle = millis();
        ledState = !ledState;
        digitalWrite(led1, ledState);
        digitalWrite(led2, !ledState);
        digitalWrite(led3, ledState);
      }
      break;

    case 2: // Both ON
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      break;

    case 3: // PWM fade on LED1
      for (int i = 0; i <= 255; i++) {
        analogWrite(led1, i);
        delay(5);
      }
      for (int i = 255; i >= 0; i--) {
        analogWrite(led1, i);
        delay(5);
      }
      break;
  }
}
