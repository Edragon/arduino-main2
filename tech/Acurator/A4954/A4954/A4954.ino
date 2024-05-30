// Freetronics HBridge shield demonstration

int motora1 = 5;
int motora2 = 6;
int motoraspeed = 4;

int motorb1 = 7;
int motorb2 = 8;
int motorbspeed = 9;

void setup()
{
  pinMode(motora1, OUTPUT);
  pinMode(motora2, OUTPUT);
  pinMode(motoraspeed, OUTPUT); 
  pinMode(motorb1, OUTPUT);
  pinMode(motorb2, OUTPUT);
  pinMode(motorbspeed, OUTPUT);  
  delay(5000); 
}

void allOff()
// turns both motors off
{
  digitalWrite(motora1, LOW);
  digitalWrite(motora2, LOW);
  digitalWrite(motoraspeed, LOW);
  digitalWrite(motorb1, LOW);
  digitalWrite(motorb2, LOW);
  digitalWrite(motorbspeed, LOW);  
}

void goForward(int speed)
{
  digitalWrite(motora1, HIGH);
  digitalWrite(motora2, LOW);
  digitalWrite(motoraspeed, speed);
  digitalWrite(motorb1, HIGH);
  digitalWrite(motorb2, LOW);
  digitalWrite(motorbspeed, speed);  
}

void goBackward(int speed)
{
  digitalWrite(motora1, LOW);
  digitalWrite(motora2, HIGH);
  digitalWrite(motoraspeed, speed);
  digitalWrite(motorb1, LOW);
  digitalWrite(motorb2, HIGH);
  digitalWrite(motorbspeed, speed); 
}

void turnRight(int speed)
{
  digitalWrite(motora1, LOW);
  digitalWrite(motora2, HIGH);
  digitalWrite(motoraspeed, speed);
  digitalWrite(motorb1, HIGH);
  digitalWrite(motorb2, LOW);
  digitalWrite(motorbspeed, speed); 
}

void turnLeft(int speed)
{
  digitalWrite(motora1, HIGH);
  digitalWrite(motora2, LOW);
  digitalWrite(motoraspeed, speed);
  digitalWrite(motorb1, LOW);
  digitalWrite(motorb2, HIGH);
  digitalWrite(motorbspeed, speed); 
}

void loop()
{
  goForward(255);
  delay(1000);
  turnLeft(200);
  delay(1000);
  goBackward(255);
  delay(1000);
  turnRight(200);
  delay(1000);
}
