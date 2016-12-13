// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#define LEFTENCODERPIN 20
#define RIGHTENCODERPIN 21
#define WHEELDIST 124
#define DISTPRDEGREE 0.48869219055

#include <AFMotor.h>
#include <Math.h>
#include <WiFi.h>

unsigned long t;
char ssid[] = "Dumpsty";     //  your network SSID (name) 
char pass[] = "test1234";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress ip;
WiFiServer server(9999);
WiFiClient client;
int count = 0;
int i = 0;
boolean shouldDrive = false;
String dataString = "";


volatile int leftTotal = 0;
volatile int rightTotal = 0;
bool motorLeftRun = false;
bool motorRightRun = false;
double heading = 0.0, posX = 0.0, posY = 0.0, margin = 10.0, goalX = 0.0, goalY = 0.0;
unsigned int tid;
int leftTemp, rightTemp, loopcount = 0, signalCount = 0;

void setup() {
  // set up Serial library at 9600 bps
  Serial.begin(9600);           

  // attempt to connect using WPA2 encryption:
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);
  
  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected, print out info about the connection:
  else {
    server.begin();
    Serial.println("Connected to network");
  }
  ip = WiFi.localIP();
  Serial.println(ip);
}

void loop() {

  // Reading available data from the server 
  if (!shouldDrive) {
    client = server.available();
    if (client.connected()){
      while (client.available()) {
        char c = client.read(); 
        dataString += c;
      }
    }
    if (dataString != "") {
      //Serial.println(dataString);
      
      //Serial.println(dataString.substring(0,dataString.indexOf(';')));
      goalX = dataString.substring(0,dataString.indexOf(';')).toInt();
      Serial.println(goalX);
      //Serial.println(dataString.substring(dataString.indexOf(';')+1));
      goalY = dataString.substring(dataString.indexOf(';')+1).toInt();
      Serial.println(goalY);
      shouldDrive = true;
      dataString = "";
    }
  } else {
    
    AF_DCMotor motorLeft(1);
    AF_DCMotor motorRight(2);
    pinMode(LEFTENCODERPIN, INPUT);
    attachInterrupt(3, incrementLeft, CHANGE);
    pinMode(RIGHTENCODERPIN, INPUT);
    attachInterrupt(2, incrementRight, CHANGE);
    // turn on motor
    motorLeft.setSpeed(255);
    motorRight.setSpeed(255);
  
  
    motorLeft.run(RELEASE);
    motorLeftRun = false;
    motorRight.run(RELEASE);
    motorRightRun = false;
  
    leftTemp = leftTotal;
    rightTemp = rightTotal;
    
    if(posX <= goalX + margin && posX >= goalX - margin && posY <= goalY + margin && posY >= goalY - margin){
      stopIfAtGoal();
    } else {
      driveTowardsGoal();  
    }
    delay(10);
    updatePosAndHead();
  }
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
  double deltaX = goalX - posX;
  double deltaY = goalY - posY;
  double goalHeading = atan(deltaX/deltaY);
  
  double deltaHeading = goalHeading - heading;
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

void stopIfAtGoal(){
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
  shouldDrive = false;
  dataString = "";
}

void incrementLeft(){
  leftTotal++;
}

void incrementRight(){
  rightTotal++;
}

