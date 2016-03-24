#include <SPI.h>
#include <WiFi.h>
#include <PWM.h>
#include "WebSocketClient.h"
#include "SoftReset.h"

int wheel1 = 1; int wheel1Enable = 40; int wheel1PWM = 3;
int wheel2 = 2; int wheel2Enable = 36; int wheel2PWM = 9;
int wheel3 = 3; int wheel3Enable = 32; int wheel3PWM = 6;
int wheel4 = 4; int wheel4Enable = 28; int wheel4PWM = 11;

int wheelRatio = 224;

int serverPort = 9000;

char ssid[] = "UDANO";
char pass[] = "10000001";
int keyIndex = 0;

int status = WL_IDLE_STATUS;
char serverSG[] = "haonguyen.me";
char serverVN[] = "103.7.41.111";

WiFiClient client;
WebSocketClient webSocketClient;

int32_t frequency = 20000;

boolean validateData (String data) 
{
  if (data=="1" || data=="2" || data=="3" || data=="4" || data=="5" || data=="6" || data=="7" || data=="8" || data=="9") {
    return true;
  }
  return false;
}

void setup() {
  
  SetPinFrequencySafe(wheel1PWM, frequency);
  SetPinFrequencySafe(wheel2PWM, frequency);
  SetPinFrequencySafe(wheel3PWM, frequency);
  SetPinFrequencySafe(wheel4PWM, frequency);
  
  pinMode(wheel1Enable, OUTPUT);
  pinMode(wheel2Enable, OUTPUT);
  pinMode(wheel3Enable, OUTPUT);
  pinMode(wheel4Enable, OUTPUT);
  
  digitalWrite(wheel1Enable, LOW);
  digitalWrite(wheel2Enable, LOW);
  digitalWrite(wheel3Enable, LOW);
  digitalWrite(wheel4Enable, LOW);
    
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while(true);
  }
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  //Serial.println("Connected to wifi");
  printWifiStatus();
  
  //Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  connectToCenter(serverVN, serverPort);
  //pinMode(5, OUTPUT); //save for terminator
}

void loop() {
  
  String data;
  if (status!=WL_CONNECTED) {
    stopAll();
    setup();
  } else {
    if (client.connected()) {
      webSocketClient.getData(data, NULL);
      if (data.length()>0) {
        if(validateData(data)) {
          Serial.println(data);
          control(data);
        } else if (data=="ping") {
          // polling
          Serial.println(data);
        } else if (data=="iamrobot") {
          Serial.println(data);
        }
      } else {
        polling();
      } 
    } else {
      stopAll();
      connectToCenter(serverVN, serverPort);
    }
  }
}

void polling()
{
  webSocketClient.sendData("ping", 1);
}

bool connectToCenter(char host[], int port)
{
 if (client.connect(serverVN, serverPort)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    return false;
  }

  // Handshake with the server
  webSocketClient.path = "/";
  webSocketClient.host = serverVN;
  
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
    webSocketClient.sendData("iamrobot", 1);
  } else {
    Serial.println("Handshake failed.");
    return false;  
  }
  return true;
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void control(String signal)
{
   // 1 2 3
   // 4 5 6
   // 7 8 9  
   
   //     / \
   //      |
   // 1        2
   // 3        4
//   Serial.println(signal);
   if (signal=="5") {
     //stop
     stopAll();
     return;
   }
   if (signal=="1") {
     // 45
     rotateClockwise(wheel1);
//     rotateClockwise(wheel2);
//     rotateClockwise(wheel3);
     rotateCounterClockwise(wheel4);
     return;
   }
   if (signal=="2") {
     rotateClockwise(wheel3);
     rotateCounterClockwise(wheel4);
     rotateClockwise(wheel1);
     rotateClockwise(wheel2);
     return;
   }
   if (signal=="3") {
     //45U
//     rotateCounterClockwise(wheel1);
     rotateClockwise(wheel2);
     rotateClockwise(wheel3);
//     rotateCounterClockwise(wheel4);
     return;
   }
   if (signal=="4") {
     rotateClockwise(wheel1);
     rotateCounterClockwise(wheel2);
     rotateCounterClockwise(wheel3);
     rotateCounterClockwise(wheel4);
     return;
   }
   if (signal=="6") {
     rotateCounterClockwise(wheel1);
     rotateClockwise(wheel2);
     rotateClockwise(wheel3);
     rotateClockwise(wheel4);
     return;
   }
   if (signal=="7") {
     rotateCounterClockwise(wheel1);
     rotateCounterClockwise(wheel2);
     rotateClockwise(wheel3);
     rotateCounterClockwise(wheel4);
     return;
   }
   if (signal=="8") { // Correct
     rotateCounterClockwise(wheel1);
     rotateCounterClockwise(wheel2);
     rotateCounterClockwise(wheel3);
     rotateClockwise(wheel4);
     return;
   }
   if (signal=="9") {
     rotateClockwise(wheel1);
     rotateClockwise(wheel2);
     rotateCounterClockwise(wheel3);
     rotateClockwise(wheel4);
     return;
   }
}

void stopAll() 
{
  stopWheel(wheel1);
  stopWheel(wheel2);
  stopWheel(wheel3);
  stopWheel(wheel4);
}

void stopWheel(int wheelNo)
{
  Serial.print("stop wheel "); Serial.println(wheelNo);
  switch(wheelNo) {
    case 1:
      digitalWrite(wheel1Enable, LOW);
      analogWrite(wheel1PWM, 127);
      break;
    case 2:
      digitalWrite(wheel2Enable, LOW);
      analogWrite(wheel2PWM, 127);
      break;
    case 3:
      digitalWrite(wheel3Enable, LOW);
      analogWrite(wheel3PWM, 127);
      break;
    case 4:
      digitalWrite(wheel4Enable, LOW);
      analogWrite(wheel4PWM, 127);
      break;
  }
}

void rotateClockwise(int wheelNo)
{
  Serial.print("rotate clockwise"); Serial.println(wheelNo);
  switch(wheelNo) {
    case 1:
      digitalWrite(wheel1Enable, HIGH);
      analogWrite(wheel1PWM, wheelRatio);
      break;
    case 2:
      digitalWrite(wheel2Enable, HIGH);
      analogWrite(wheel2PWM, wheelRatio);
      break;
    case 3:
      digitalWrite(wheel3Enable, HIGH);
      analogWrite(wheel3PWM, wheelRatio);
      break;
    case 4:
      digitalWrite(wheel4Enable, HIGH);
      analogWrite(wheel4PWM, wheelRatio);
      break;
  }
}

void rotateCounterClockwise(int wheelNo)
{
  Serial.print("rotate counter clockwise "); Serial.println(wheelNo);
  switch(wheelNo) {
    case 1:
      digitalWrite(wheel1Enable, HIGH);
      analogWrite(wheel1PWM, 256-wheelRatio);
      break;
    case 2:
      digitalWrite(wheel2Enable, HIGH);
      analogWrite(wheel2PWM, 256-wheelRatio);
      break;
    case 3:
      digitalWrite(wheel3Enable, HIGH);
      analogWrite(wheel3PWM, 256-wheelRatio);
      break;
    case 4:
      digitalWrite(wheel4Enable, HIGH);
      analogWrite(wheel4PWM, 256-wheelRatio);
      break;
  }
}
