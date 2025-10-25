const int buttonPin = 12; 

const int ledPin = 17; 

volatile bool ledState = LOW; 

 
 

void IRAM_ATTR handleButton() { 

  ledState = !ledState; 

  digitalWrite(ledPin, ledState); 

} 

 
 

void setup() { 

  pinMode(buttonPin, INPUT_PULLUP); 

  pinMode(ledPin, OUTPUT); 

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, FALLING); 

} 

 
 

void loop() { 

  // main loop free for other tasks 

} 

 