// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  bootM4();
  pinMode(LEDR, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LEDR, LOW); // turn the red LED on (LOW is the voltage level)
  delay(200); // wait for 200 milliseconds
  digitalWrite(LEDR, HIGH); // turn the LED off by setting the voltage HIGH
  delay(200); // wait for 200 milliseconds
}
