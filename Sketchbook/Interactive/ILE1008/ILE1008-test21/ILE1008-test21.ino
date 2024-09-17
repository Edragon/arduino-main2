int b = 13;
int r = 12;
int g = 11;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(b, OUTPUT);
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  checking(b);
  checking(r);
  checking(g);
}

void checking(int led) {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                       // wait for a second
}
