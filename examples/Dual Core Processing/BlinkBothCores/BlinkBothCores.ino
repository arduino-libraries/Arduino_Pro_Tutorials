int myLED;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(0));

#ifdef CORE_CM7
  bootM4();
  myLED = LEDB; // built-in blue LED
#endif
#ifdef CORE_CM4
  myLED = LEDG; // built-in greeen LED
#endif
  pinMode(myLED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(myLED, LOW); // turn the LED on
  delay(200);
  digitalWrite(myLED, HIGH); // turn the LED off
  delay( rand() % 2000 + 1000); // wait for a random amount of time between 1 and 3 seconds.

}
