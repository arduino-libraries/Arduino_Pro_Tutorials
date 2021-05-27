#include <Arduino_EdgeControl.h>

void setup() {
  Serial.begin(9600);

  // Set the timeout
  auto startNow = millis() + 2500;
  while (!Serial && millis() < startNow);
  Serial.println("Hello, Edge Control Sketch!");

  // Enable power lines 
  Power.enable3V3();
  Power.enable5V();

  // Start the I2C connection 
  Wire.begin();

  // Initialize the expander pins 
  Expander.begin();
  Expander.pinMode(EXP_LED1, OUTPUT);
}

void loop() {    
    // put your main code here, to run repeatedly:
    Serial.println("Blink");
    Expander.digitalWrite(EXP_LED1, LOW);
    delay(500);
    Expander.digitalWrite(EXP_LED1, HIGH);
    delay(500);
}