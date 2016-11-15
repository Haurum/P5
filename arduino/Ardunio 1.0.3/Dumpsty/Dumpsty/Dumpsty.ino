// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#define LEFTENCODERPIN 20
#define RIGHTENCODERPIN 21
#define LEFTPINTOINTERRUPT 3
#define RIGHTPINTOINTERRUPT 2
#define WHEELDIST 124
#define DISTPRDEGREE 0.48869219055

#include <AFMotor.h>
#include <Math.h>
#include <WiFi.h>

// Variables for the motorcontrol, turning and positioning
volatile int leftTotal = 0;
volatile int rightTotal = 0;
bool motorLeftRun = false;
bool motorRightRun = false;
double heading = 0.0, posX = 0.0, posY = 0.0;
unsigned int tid;
int leftTemp, rightTemp, goalX = 230, goalY = 330, margin = 10, loopcount = 0, signalCount = 0;

AF_DCMotor motorLeft(1);
AF_DCMotor motorRight(2);

// Variables for the Wifi-connection
char ssid[] = "Dumpsty";    
char pass[] = "test1234";   
int status = WL_IDLE_STATUS;
IPAddress ip;
WiFiServer server(9999);
WiFiClient client;
int count = 0;
int i = 0;
String dataString = "";

void setup() {
  Serial.begin(9600);

  // Printing Wifi-status when connecting
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  else {
    server.begin();
    Serial.println("Connected to network");
  }
  ip = WiFi.localIP();
  Serial.println(ip);

  pinMode(LEFTENCODERPIN, INPUT);
  attachInterrupt(LEFTPINTOINTERRUPT, incrementLeft, CHANGE);
  pinMode(RIGHTENCODERPIN, INPUT);
  attachInterrupt(RIGHTPINTOINTERRUPT, incrementRight, CHANGE);
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
  loopcount++;
  
  // Reading available data from the server
  client = server.available();
  while (client.connected()){
    if (client.available()) {
      char c = client.read(); 
      dataString += c;
    }
  }
  
  if(loopcount % 5 == 0 && signalCount < 5){
    signalCount++;
    goalX += -50;
    goalY += 10;
  }
  if(round(posX) <= goalX + margin && round(posX) >= goalX - margin && round(posY) <= goalY + margin && round(posY) >= goalY - margin){
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
    delay(50);
    exit(0);
  }
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
  delay(10);
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

void incrementLeft(){
  leftTotal++;
}

void incrementRight(){
  rightTotal++;
}

