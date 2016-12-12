#include <ArduinoUnit.h>

extern "C"{
  #include <avrfix.h>
}

// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#define LEFTENCODERPIN 20
#define RIGHTENCODERPIN 21
#define WHEELDIST 124
#define DISTPRDEGREE 0.48869

bool atGoal = false;
fix_t heading, posX, posY, margin, goalX, goalY; 
int leftTemp, rightTemp, counter, leftTotal, rightTotal;
unsigned long t, startTime;




void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
}

void loop() {

  Test::run();
  
}

test(curPos30_150goalPos30_150Head1){
  heading = itok(1);
  posX = itok(30);
  posY = itok(150);
  goalX = itok(30);
  goalY = itok(150);
  String result = driveTowardsGoal();
  assertEqual(result, "goal");
}

test(curPos0_0goalPos0_150Head0){
  heading = itok(0);
  posX = itok(0);
  posY = itok(0);
  goalX = itok(0);
  goalY = itok(150);
  String result = driveTowardsGoal();
  assertEqual(result, "forward");
}

test(curPos0_0goalPos0_150negHead1){
  heading = itok(1);
  posX = itok(0);
  posY = itok(0);
  goalX = itok(0);
  goalY = itok(-150);
  String result = driveTowardsGoal();
  assertEqual(result, "left");
}

test(curPos150_150goalPos140_100Head1neg){
  heading = itok(-1);
  posX = itok(150);
  posY = itok(150);
  goalX = itok(140);
  goalY = itok(100);
  String result = driveTowardsGoal();
  assertEqual(result, "right");
}

test(curPos150_150goalPos300_200Head1neg){
  heading = itok(-1);
  posX = itok(150);
  posY = itok(150);
  goalX = itok(300);
  goalY = itok(200);
  String result = driveTowardsGoal();
  assertEqual(result, "right");
}

test(curPos0_0goalPos0_300Head1){
  heading = itok(1);
  posX = itok(0);
  posY = itok(0);
  goalX = itok(0);
  goalY = itok(300);
  String result = driveTowardsGoal();
  assertEqual(result, "right");
}

test(curPos0_0goalPos150neg_300Head0){
  heading = itok(0);
  posX = itok(0);
  posY = itok(0);
  goalX = itok(-150);
  goalY = itok(300);
  String result = driveTowardsGoal();
  assertEqual(result, "left");
}

test(pos300_250Head075Left12Right5){
  heading = ftok(0.75);
  posX = itok(300);
  posY = itok(250);
  rightTotal = 5;
  rightTemp = 0;
  leftTotal = 12;
  leftTemp = 0;
  
  updatePosAndHead();
  
  assertMore(posX, ftok(302.826));
  assertLess(posX, ftok(302.836)); 
   
  assertMore(posY, ftok(253.034));
  assertLess(posY, ftok(253.044));
  
  assertMore(heading, ftok(0.717));
  assertLess(heading, ftok(0.727));  
}

test(pos160neg_100Head04negLeft3Right14){
  heading = ftok(-0.4);
  posX = itok(-160);
  posY = itok(100);
  rightTotal = 14;
  rightTemp = 0;
  leftTotal = 3;
  leftTemp = 0;
  
  updatePosAndHead();
  assertMore(posX, ftok(-161.623));
  assertLess(posX, ftok(-161.613)); 
   
  assertMore(posY, ftok(103.821));
  assertLess(posY, ftok(103.831));
  
  assertMore(heading, ftok(-0.365));
  assertLess(heading, ftok(-0.355));  
}

test(pos120_200Head1Left10Right10){
  heading = itok(1);
  posX = itok(120);
  posY = itok(200);
  rightTotal = 10;
  rightTemp = 0;
  leftTotal = 10;
  leftTemp = 0;
  
  updatePosAndHead();
  
  assertMore(posX, ftok(124.107));
  assertLess(posX, ftok(124.117)); 
   
  assertMore(posY, ftok(202.635));
  assertLess(posY, ftok(202.645));

  assertMore(heading, ftok(0.995));
  assertLess(heading, ftok(1.005));  
}

test(pos0_0Head0Left0Right10){
  heading = itok(0);
  posX = itok(0);
  posY = itok(0);
  rightTotal = 10;
  rightTemp = 0;
  leftTotal = 0;
  leftTemp = 0;
  
  updatePosAndHead();
  
  assertEqual(posX, itok(0));
  
  assertMore(posY, ftok(2.438));
  assertLess(posY, ftok(2.448));

  assertMore(heading, ftok(0.034));
  assertLess(heading, ftok(0.044));  
}

test(pos0_0Head0Left10Right0){
  heading = itok(0);
  posX = itok(0);
  posY = itok(0);
  rightTotal = 0;
  rightTemp = 0;
  leftTotal = 10;
  leftTemp = 0;
  
  updatePosAndHead();
  
  assertEqual(posX, itok(0));
  
  assertMore(posY, ftok(2.438));
  assertLess(posY, ftok(2.448));
  
  assertMore(heading, ftok(-0.044));
  assertLess(heading, ftok(-0.034));
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

String driveTowardsGoal(){
  if(atGoal || (posX <= goalX + margin && posX >= goalX - margin && posY <= goalY + margin && posY >= goalY - margin)){
    return "goal";
  }else{
    fix_t deltaX = goalX - posX;
    fix_t deltaY = goalY - posY;
    fix_t actualGoalHeading;
    if(deltaX == itok(0) && deltaY > itok(0))
      actualGoalHeading = itok(0);
    else if(deltaX == itok(0) && deltaY < itok(0))
      actualGoalHeading = PIk;
    else{
      fix_t rel = divk(deltaY,deltaX);
      fix_t goalHeading = atank(rel);
      actualGoalHeading = deltaX >= itok(0) ? goalHeading-(PIk/2) : goalHeading+(PIk/2);
    }
    fix_t deltaHeading = actualGoalHeading - heading;
    if(deltaHeading > PIk)
      deltaHeading -= 2*PIk;
     else if(deltaHeading < -PIk)
      deltaHeading += 2*PIk;
    if(deltaHeading < ftok(-0.1)){
      return "right";
    }else if(deltaHeading > ftok(0.1)){
      return "left";
    }else{
      return "forward";
    }
  }
}

void incrementLeft(){
  leftTotal++;
}

void incrementRight(){
  rightTotal++;
}

