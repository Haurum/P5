extern "C"{
  #include <avrfix.h>
}
#include <AFMotor.h>
#include <Math.h>

// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#define LEFTENCODERPIN 20
#define RIGHTENCODERPIN 21
#define WHEELDIST 124
#define DISTPRDEGREE 0.48869219055

volatile int leftTotal = 0;
volatile int rightTotal = 0;
bool atGoal = false;
fix_t heading, posX, posY, margin, goalX, goalY; 
int leftTemp, rightTemp, counter;
unsigned long t, startTime;
#include <AFMotor.h>
#include <Math.h>

AF_DCMotor motorLeft(1);
AF_DCMotor motorRight(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps

  pinMode(LEFTENCODERPIN, INPUT);
  attachInterrupt(3, incrementLeft, CHANGE);
  pinMode(RIGHTENCODERPIN, INPUT);
  attachInterrupt(2, incrementRight, CHANGE);

  heading = itok(0);
  posX = itok(0);
  posY = itok(0);
  margin = itok(10);
  goalX = itok(-250);
  goalY = itok(300);
  
  // turn on motor
  motorLeft.setSpeed(255);
  motorRight.setSpeed(255);

  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);

  leftTemp = leftTotal;
  rightTemp = rightTotal;

  t = millis();
  startTime = t;
}

void loop() {
  
  getNewGoal();
  
  updatePosAndHead();
  
  driveTowardsGoal();
  
}

void getNewGoal(){
  delay(9);
  if((millis() - t) > 33 && counter < 50){
    goalX = itok(10) + goalX;
    counter++;
    atGoal = false; 
    t = millis();
  }else if (counter == 50){
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
    delay(50);
    exit(0);
  }
}

void updatePosAndHead(){
  int currentLeft = leftTotal;
  int currentRight = rightTotal;
  fix_t distPrDeg = ftok(DISTPRDEGREE);
  fix_t dltL = itok(currentLeft - leftTemp);
  fix_t dltR = itok(currentRight - rightTemp);
  fix_t deltaLeft = mulk(dltL, distPrDeg);
  fix_t deltaRight = mulk(dltR, distPrDeg);
  leftTemp = currentLeft;
  rightTemp = currentRight;
  fix_t deltaSum = deltaLeft + deltaRight;
  fix_t dist = divk(deltaSum,ftok(2.0));
  fix_t sinHeading = sink(heading);
  posX += mulk(dist, sinHeading);
  fix_t cosHeading = cosk(heading);
  posY += mulk(dist, cosHeading);
  fix_t rel = divk((deltaRight - deltaLeft),ftok(WHEELDIST));
  heading += atank(rel);
}

void driveTowardsGoal(){
  if(atGoal || (posX <= goalX + margin && posX >= goalX - margin && posY <= goalY + margin && posY >= goalY - margin)){
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
    atGoal = true;
  }else{
    fix_t deltaX = goalX - posX;
    fix_t deltaY = goalY - posY;
    fix_t rel = divk(deltaX,deltaY);
    fix_t goalHeading = atank(rel);
    fix_t deltaHeading = goalHeading - heading;
    if(deltaHeading < ftok(-0.1)){
      motorLeft.run(FORWARD);
      motorRight.run(RELEASE);
    }else if(deltaHeading > ftok(0.1)){
      motorLeft.run(RELEASE);
      motorRight.run(FORWARD);
    }else{
      motorLeft.run(FORWARD);
      motorRight.run(FORWARD);
    }
  }
}

void incrementLeft(){
  leftTotal++;
}

void incrementRight(){
  rightTotal++;
}

