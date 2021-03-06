#define LEFTENCODERPIN 20
#define RIGHTENCODERPIN 21
#define WHEELDIST 124
#define DISTPRDEGREE 0.48869219055

#include <AFMotor.h>
#include <Math.h>
#include <WiFi.h>

char ssid[] = "Dumpsty";     //  your network SSID (name) 
char pass[] = "test1234";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress ip;
WiFiServer server(9999);
WiFiClient client;
String dataString = "";

unsigned long t;
volatile int leftTotal = 0;
volatile int rightTotal = 0;
double heading = 0.0, posX = 0.0, posY = 0.0, margin = 10.0, goalX = 0.0, goalY = 0.0;
int leftTemp, rightTemp;

void setup() {
  Serial.begin(9600);           
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    exit(0);
  } 
  else {
    server.begin();
  }
  ip = WiFi.localIP();

  while(dataString == ""){
    client = server.available();
    if (client.connected()){
      while (client.available()) {
        char c = client.read(); 
        dataString += c;
      }
    }
  }
  goalX = dataString.substring(0,dataString.indexOf(';')).toInt();
  goalY = dataString.substring(dataString.indexOf(';')+1).toInt();  

  pinMode(LEFTENCODERPIN, INPUT);
  attachInterrupt(3, incrementLeft, CHANGE);
  pinMode(RIGHTENCODERPIN, INPUT);
  attachInterrupt(2, incrementRight, CHANGE);

  leftTemp = leftTotal;
  rightTemp = rightTotal;
}

void loop() {
    t = millis();
    wifi();
    updatePosAndHead();
    driveTowardsGoal();  
    delay(10);
    updatePosAndHead();
    driveTowardsGoal();
    while(micros()-t<33){}  
}

void wifi(){
  delay(9);
}

void updatePosAndHead(){
  int currentLeft = leftTotal;
  int currentRight = rightTotal;
  double deltaLeft = (currentLeft - leftTemp) * DISTPRDEGREE;
  double deltaRight = (currentRight - rightTemp) * DISTPRDEGREE;
  leftTemp = currentLeft;
  rightTemp = currentRight;
  double dist = (deltaLeft + deltaRight) / 2.0;
  posX += (dist * sin(heading));
  posY += (dist * cos(heading));
  heading += (atan((deltaRight - deltaLeft) / WHEELDIST));
}

void driveTowardsGoal(){
  AF_DCMotor motorLeft(1);
  AF_DCMotor motorRight(2);
  
  motorLeft.setSpeed(255);
  motorRight.setSpeed(255);
  
  if(posX <= goalX + margin && posX >= goalX - margin && posY <= goalY + margin && posY >= goalY - margin){
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
    delay(50);
    exit(0);
  } else {
    double deltaX = goalX - posX;
    double deltaY = goalY - posY;
    double actualGoalHeading;
    if(deltaX == 0 && deltaY > 0)
      actualGoalHeading = 0;
    else if(deltaX == 0 && deltaY < 0)
      actualGoalHeading = PI;
    else
      actualGoalHeading = deltaX >= 0 ? atan(deltaY/deltaX) - (PI/2) : atan(deltaY/deltaX) + (PI/2);
    
    double deltaHeading = actualGoalHeading - heading;
    if(deltaHeading > PI)
      deltaHeading -= 2*PI;
    else if(deltaHeading < -PI)
      deltaHeading += 2*PI;
    if(deltaHeading < -0.1){
      motorLeft.run(FORWARD);
      motorRight.run(RELEASE);
    }else if(deltaHeading > 0.1){
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

