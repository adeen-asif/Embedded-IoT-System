#include <Arduino.h>

#define BUZZER_PIN  27     // GPIO connected to buzzer
#define PWM_CH      0      // PWM channel (0–15)
#define FREQ        2000   // Default frequency (Hz)
#define RESOLUTION  10     // 10-bit resolution (0–1023)

#define LED1_PIN 18
#define PWM1_CH 1
#define FREQ1 5000
#define RES1 8

#define LED2_PIN 19
#define PWM2_CH 2
#define FREQ2 2000
#define RES2 10

void setup() {
  // Setup PWM channel
  ledcSetup(PWM_CH, FREQ, RESOLUTION);
  ledcAttachPin(BUZZER_PIN, PWM_CH);

  ledcSetup(PWM1_CH, FREQ1, RES1);
  ledcAttachPin(LED1_PIN, PWM1_CH);

  ledcSetup(PWM2_CH, FREQ2, RES2);
  ledcAttachPin(LED2_PIN, PWM2_CH);
}
 

void loop() {
  // --- 1. Simple beep pattern ---
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(PWM_CH, 2000 + i * 400); // Change tone
    ledcWrite(PWM1_CH, 255);               // Turn on LED1
    delay(150);
    ledcWrite(PWM_CH, 0);                  // Stop tone
    ledcWrite(PWM1_CH, 0);                 // Turn off LED1
    delay(150);
  }

  // --- 2. Frequency sweep (400Hz → 3kHz) ---
  for (int f = 400; f <= 3000; f += 100) {
    ledcWriteTone(PWM_CH, f);
    ledcWrite(PWM2_CH, (f / 12));          // LED2 brightness changes with tone
    delay(20);
  }
  ledcWrite(PWM_CH, 0);
  ledcWrite(PWM2_CH, 0);
  delay(500);

  // --- 3. Short melody ---
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    ledcWriteTone(PWM_CH, melody[i]);
    ledcWrite(PWM1_CH, 255);               // Flash LED1 during note
    delay(250);
    ledcWrite(PWM1_CH, 0);
    delay(100);
  }

  // --- 4. Buzzer fade-in and fade-out ---
  for (int d = 0; d <= 255; d++) { 
    ledcWrite(PWM2_CH, d); 
    delay(10); 
  }
  for (int d = 255; d >= 0; d--) { 
    ledcWrite(PWM2_CH, d); 
    delay(10); 
  }

  ledcWrite(PWM_CH, 0);  // Stop buzzer
  delay(1000);           // Wait before repeating
}