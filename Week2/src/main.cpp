// #include <Arduino.h>

// #define LED1 2
// #define LED2 3
// #define BUTTON 4

// int ledState1 = LOW;
// int ledState2 = LOW;

// int buttonState = 0;
// int lastButtonState = 0;

// int blinkDelay = 500; // default blink delay
// int pattern = 0;      // pattern selector

// void setup() {
//   pinMode(LED1, OUTPUT);
//   pinMode(LED2, OUTPUT);
//   pinMode(BUTTON, INPUT);
// }

// void loop() {
//   buttonState = digitalRead(BUTTON);

//   // Detect button press (toggle on release)
//   if (buttonState != lastButtonState && buttonState == HIGH) {
//     pattern++; // change pattern on each press
//     if (pattern > 3) pattern = 0; // loop patterns
//   }
//   lastButtonState = buttonState;

//   // Execute patterns
//   switch(pattern) {
//     case 0: // both LEDs blink together
//       digitalWrite(LED1, HIGH);
//       digitalWrite(LED2, HIGH);
//       delay(blinkDelay);
//       digitalWrite(LED1, LOW);
//       digitalWrite(LED2, LOW);
//       delay(blinkDelay);
//       break;

//     case 1: // alternate LEDs
//       digitalWrite(LED1, HIGH);
//       digitalWrite(LED2, LOW);
//       delay(blinkDelay);
//       digitalWrite(LED1, LOW);
//       digitalWrite(LED2, HIGH);
//       delay(blinkDelay);
//       break;

//     case 2: // LED1 faster than LED2
//       digitalWrite(LED1, HIGH);
//       delay(blinkDelay / 2);
//       digitalWrite(LED1, LOW);
//       delay(blinkDelay / 2);

//       digitalWrite(LED2, HIGH);
//       delay(blinkDelay);
//       digitalWrite(LED2, LOW);
//       delay(blinkDelay);
//       break;

//     case 3: // toggle both LEDs on button press
//       digitalWrite(LED1, ledState1);
//       digitalWrite(LED2, ledState2);
//       ledState1 = !ledState1;
//       ledState2 = !ledState2;
//       delay(500);
//       break;
//   }
// }


#include <Arduino.h>

const int buttonPin = 21;                       // the number of the pushbutton pin
const int ledPin =  2;                        // the number of the LED pin
int buttonState = 0;                           // variable for reading the pushbutton status
void setup() {
    pinMode(ledPin, OUTPUT);                   // initialize the LED pin as an output:
    pinMode(buttonPin, INPUT_PULLUP);          // initialize the pushbutton pin as an input:
}                                               
void loop() {
   buttonState = digitalRead(buttonPin);       // read the state of the pushbutton value:
   if (buttonState == HIGH) {                  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
       digitalWrite(ledPin, HIGH);             // turn LED on:
   } else {
       digitalWrite(ledPin, LOW);              // turn LED off:
   }
}
