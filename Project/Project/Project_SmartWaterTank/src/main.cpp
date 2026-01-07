#define BLYNK_TEMPLATE_ID "TMPL6zsB221T7"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring"
#define BLYNK_AUTH_TOKEN "_z2nnEhTZNoeWcZi6i60JxPexzv4fUP2"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <algorithm> 

char ssid[] = "Wireless";
char pass[] = "100200300";

#define LED_0 33
#define LED_25 32
#define LED_50 26
#define LED_75 25
#define LED_100 14
#define RELAY_PIN 13
#define BUZZER_PIN 4

#define BUTTON_MODE 27
#define BUTTON_PUMP_ON 34
#define BUTTON_PUMP_OFF 35

#define US1_TRIG 5
#define US1_ECHO 18
#define US2_TRIG 19
#define US2_ECHO 23
#define US3_TRIG 17
#define US3_ECHO 16

Adafruit_SSD1306 display(128, 64, &Wire, -1);
BlynkTimer timer;

enum Mode { AUTOMATIC, MANUAL };
Mode currentMode = AUTOMATIC;
volatile bool pumpState = false;

float smoothedLevel = 0.0; 
float filterFactor = 0.15; 
int finalWaterLevel = 0;

#define DEBOUNCE_MS 300  
#define DEBOUNCE_US (DEBOUNCE_MS * 1000UL)

hw_timer_t* timerMode = nullptr;
hw_timer_t* timerPumpOn = nullptr;
hw_timer_t* timerPumpOff = nullptr;

volatile bool debounceModeActive = false;
volatile bool debouncePumpOnActive = false;
volatile bool debouncePumpOffActive = false;

float getMedianDistance(int trig, int echo) {
  float r[3];
  for (int i = 0; i < 3; i++) {
    digitalWrite(trig, LOW); delayMicroseconds(2);
    digitalWrite(trig, HIGH); delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long duration = pulseIn(echo, HIGH, 25000);
    r[i] = (duration == 0) ? 11.0 : (duration * 0.0343) / 2.0;
    delay(10);
  }
  std::sort(r, r + 3);
  return r[1];
}

void calculateWaterLevel() {
  float d1 = getMedianDistance(US1_TRIG, US1_ECHO);
  float d2 = getMedianDistance(US2_TRIG, US2_ECHO);
  float d3 = getMedianDistance(US3_TRIG, US3_ECHO);

  float readings[] = {d1, d2, d3};
  float sum = 0;
  int count = 0;

  for (int i = 0; i < 3; i++) {
    if (readings[i] >= 1.5 && readings[i] <= 15.0) {
      sum += readings[i];
      count++;
    }
  }

  float avgDist = (count > 0) ? (sum / count) : 11.0;
  int rawLevel = map(avgDist * 10, 20, 110, 100, 0);
  rawLevel = constrain(rawLevel, 0, 100);

  smoothedLevel = (rawLevel * filterFactor) + (smoothedLevel * (1.0 - filterFactor));
  finalWaterLevel = (int)(smoothedLevel + 0.5);
}

void handleAutomaticMode() {
  if (finalWaterLevel <= 10) {
    pumpState = true; 
  } else if (finalWaterLevel >= 90) {
    pumpState = false;
  }
}

void updateHardware() {
  digitalWrite(LED_0,   finalWaterLevel >= 5);
  digitalWrite(LED_25,  finalWaterLevel >= 25);
  digitalWrite(LED_50,  finalWaterLevel >= 50);
  digitalWrite(LED_75,  finalWaterLevel >= 75);
  digitalWrite(LED_100, finalWaterLevel >= 95);

  digitalWrite(RELAY_PIN, pumpState ? LOW : HIGH);

  if (finalWaterLevel < 10 || finalWaterLevel > 90)
    digitalWrite(BUZZER_PIN, HIGH);
  else
    digitalWrite(BUZZER_PIN, LOW);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("WATER LEVEL SYSTEM");
  display.println("--------------------");
  display.printf("Mode: %s\n", currentMode == AUTOMATIC ? "Automatic" : "Manual");
  display.printf("Level: %d%%\n", finalWaterLevel);
  display.printf("Pump: %s\n", pumpState ? "ON" : "OFF");

  display.display();
}

void sendToBlynk() {
  Blynk.virtualWrite(V0, finalWaterLevel);
  Blynk.virtualWrite(V2, currentMode == AUTOMATIC ? 1 : 0);
  Blynk.virtualWrite(V3, pumpState ? 1 : 0);
  Blynk.virtualWrite(V4, (finalWaterLevel < 10 || finalWaterLevel > 90) ? 1 : 0);

  static bool highSent = false;
  static bool lowSent = false;

  if (finalWaterLevel >= 90 && !highSent) {
    Blynk.logEvent("water_high", "⛔ Water Level HIGH!");
    highSent = true;
    lowSent = false;
  }

  if (finalWaterLevel <= 15 && !lowSent) {
    Blynk.logEvent("water_low", "⚠️ Water Level LOW!");
    lowSent = true;
    highSent = false;
  }
}

BLYNK_WRITE(V2) {
  int val = param.asInt();
  currentMode = (val == 1) ? AUTOMATIC : MANUAL;
  if (currentMode == AUTOMATIC && finalWaterLevel < 90) {
    pumpState = true;
  }
  updateHardware();
  Blynk.virtualWrite(V3, pumpState ? 1 : 0);
}

BLYNK_WRITE(V3) {
  if (currentMode == MANUAL) {
    pumpState = (param.asInt() == 1);
    updateHardware();
  } else {
    Blynk.virtualWrite(V3, pumpState ? 1 : 0);
  }
}

void IRAM_ATTR onModeISR() {
  if (!debounceModeActive) {
    debounceModeActive = true;
    timerAlarmWrite(timerMode, DEBOUNCE_US, false);
    timerAlarmEnable(timerMode);
  }
}

void IRAM_ATTR onPumpOnISR() {
  if (!debouncePumpOnActive) {
    debouncePumpOnActive = true;
    timerAlarmWrite(timerPumpOn, DEBOUNCE_US, false);
    timerAlarmEnable(timerPumpOn);
  }
}

void IRAM_ATTR onPumpOffISR() {
  if (!debouncePumpOffActive) {
    debouncePumpOffActive = true;
    timerAlarmWrite(timerPumpOff, DEBOUNCE_US, false);
    timerAlarmEnable(timerPumpOff);
  }
}

void IRAM_ATTR modeDebounceTimer() {
  if (digitalRead(BUTTON_MODE) == HIGH) {
    currentMode = (currentMode == AUTOMATIC) ? MANUAL : AUTOMATIC;
    if (currentMode == AUTOMATIC && finalWaterLevel < 90)
      pumpState = true;
  }
  debounceModeActive = false;
}

void IRAM_ATTR pumpOnDebounceTimer() {
  if (currentMode == MANUAL && digitalRead(BUTTON_PUMP_ON) == HIGH)
    pumpState = true;
  debouncePumpOnActive = false;
}

void IRAM_ATTR pumpOffDebounceTimer() {
  if (currentMode == MANUAL && digitalRead(BUTTON_PUMP_OFF) == HIGH)
    pumpState = false;
  debouncePumpOffActive = false;
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_0, OUTPUT); pinMode(LED_25, OUTPUT); pinMode(LED_50, OUTPUT);
  pinMode(LED_75, OUTPUT); pinMode(LED_100, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);

  pinMode(BUTTON_MODE, INPUT);
  pinMode(BUTTON_PUMP_ON, INPUT);
  pinMode(BUTTON_PUMP_OFF, INPUT);

  pinMode(US1_TRIG, OUTPUT); pinMode(US1_ECHO, INPUT);
  pinMode(US2_TRIG, OUTPUT); pinMode(US2_ECHO, INPUT);
  pinMode(US3_TRIG, OUTPUT); pinMode(US3_ECHO, INPUT);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timerMode = timerBegin(0, 80, true);
  timerAttachInterrupt(timerMode, &modeDebounceTimer, true);

  timerPumpOn = timerBegin(1, 80, true);
  timerAttachInterrupt(timerPumpOn, &pumpOnDebounceTimer, true);

  timerPumpOff = timerBegin(2, 80, true);
  timerAttachInterrupt(timerPumpOff, &pumpOffDebounceTimer, true);

  attachInterrupt(BUTTON_MODE, onModeISR, RISING);
  attachInterrupt(BUTTON_PUMP_ON, onPumpOnISR, RISING);
  attachInterrupt(BUTTON_PUMP_OFF, onPumpOffISR, RISING);

  timer.setInterval(500L, []() {
    calculateWaterLevel();
    if (currentMode == AUTOMATIC) handleAutomaticMode();
    updateHardware();
    updateDisplay();
    sendToBlynk();
  });
}

void loop() {
  Blynk.run();
  timer.run();
}