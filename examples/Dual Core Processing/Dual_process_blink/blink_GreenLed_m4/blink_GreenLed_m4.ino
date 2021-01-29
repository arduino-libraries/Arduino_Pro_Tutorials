// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LEDG, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    digitalWrite(LEDG, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(500); // wait for a second
    digitalWrite(LEDG, LOW); // turn the LED off by making the voltage LOW
    delay(500); // wait for a second
}
