#include <WiFi.h>

char ssid[] = "Dumpsty";     //  your network SSID (name) 
char pass[] = "test1234";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress ip;
WiFiServer server(9999);
WiFiClient client;
int message;

void setup() {
  // initialize serial:
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
  delay(1000);
  client = server.available();
  if(!client){
    message = 0;
  }
  else{
    message = 1;
  }
  Serial.println(message);
  client.stop();
}
