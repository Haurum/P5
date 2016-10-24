// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#define LEFTENCODERPIN 20
#define RIGHTENCODERPIN 21
#define WHEELDIST 124
#define DISTPRDEGREE 0.48869219055

volatile int leftTotal = 0;
volatile int rightTotal = 0;
bool motorLeftRun = false;
bool motorRightRun = false;
double degree, gennemsnit, deltaHeading, heading = 0.0, posX = 0.0, posY = 0.0;
unsigned int tid;
double wheelDistInDeg = WHEELDIST/DISTPRDEGREE;
int leftTemp, rightTemp, goalX = 125, goalY = 255, margin = 5;

#include <AFMotor.h>
#include <Math.h>

AF_DCMotor motorLeft(1);
AF_DCMotor motorRight(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps

  pinMode(LEFTENCODERPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(LEFTENCODERPIN), incrementLeft, CHANGE);
  pinMode(RIGHTENCODERPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RIGHTENCODERPIN), incrementRight, CHANGE);
  // turn on motor
  motorLeft.setSpeed(255);
  motorRight.setSpeed(255);


  motorLeft.run(RELEASE);
  motorLeftRun = false;
  motorRight.run(RELEASE);
  motorRightRun = false;

  leftTemp = leftTotal;
  rightTemp = rightTotal;
  
}

void loop() {
  
  if(round(posX) <= goalX + margin && round(posX) >= goalX - margin && round(posY) <= goalY + margin && round(posY) >= goalY - margin){
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
    delay(50);
    exit(0);
  }
}

void incrementLeft(){
  leftTotal++;
}

void incrementRight(){
  rightTotal++;
}

