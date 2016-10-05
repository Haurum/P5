// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#define BLUEPIN 52
#define YELLOWPIN 53

int total = 0, offset;
double degree, gennemsnit;
unsigned long tid;

#include <AFMotor.h>

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps

  pinMode(BLUEPIN, INPUT);
  pinMode(YELLOWPIN, INPUT);
  // turn on motor
  motor1.setSpeed(100);
  motor2.setSpeed(255);

  motor2.run(RELEASE);

  delay(5000);

  motor2.run(FORWARD);

  tid = millis();
}

void loop() {

  if(millis() - tid >= 2500){
    motor2.run(RELEASE);
    exit(0);
  }
  
  
}
