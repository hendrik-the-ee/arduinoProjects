/*
  HG Intevalometer
  HG rewritten 2017/08/20
 */


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(12, OUTPUT);  // focus
  pinMode(13, OUTPUT);  // picture
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(800);              // wait
  digitalWrite(13, HIGH);   // turn the LED off by making the voltage LOW
  delay(200);               // wait for a second
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  delay(29000);
}
