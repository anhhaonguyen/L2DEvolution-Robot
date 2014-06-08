#include <SPI.h>
#include <WiFi.h>
#include "WebSocketClient.h"

char ssid[] = "LOZI";
char pass[] = "kjnhkHungvl1";
int keyIndex = 0;

int status = WL_IDLE_STATUS;
char server[] = "192.168.1.104";

WiFiClient client;
WebSocketClient webSocketClient;

int speedValue = 2;

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void setup() {
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
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  connectToCenter(server, 3005);
}

void loop() {
  
  String data;
  if (client.connected()) {
    webSocketClient.getData(data, NULL);
    if (data.length() > 0) {
      //Hao Nguyen TO DO
      control(data);
    } else {
      polling();
    }
  } else {
    Serial.println('Disconnect!');
    connectToCenter(server, 3005);
  }  
  
}

void control(String signal)
{
   if(signal=="s" || signal=="s"){
      Serial.write("stop");
      analogWrite(3, 0);
      analogWrite(9, 0);
      return;
    }
    if(signal=="f" || signal=="F"){
      Serial.write("go forward");
      analogWrite(3, 16*speedValue);
      analogWrite(9, 0);
      return;
    }
    if(signal=="b" || signal=="B"){
      Serial.write("go backward");
      analogWrite(3, 0);
      analogWrite(9, 16*speedValue);
      return;
    }
    if(signal=="r" || signal=="R"){
      Serial.write("turn right");
      analogWrite(5, 128);
      analogWrite(6, 128);
      return;
    }
    if(signal=="l" || signal=="L"){
      Serial.write("turn left");
      analogWrite(5, 128);
      analogWrite(6, 0);
      return;
    }
    if(signal=="s" || signal=="S"){
      Serial.write("stop");
      analogWrite(3, 0);
      analogWrite(9, 0);
      return;
    }
    if(signal=="u" || signal=="U"){
      Serial.write("increase speed");
      speedValue++;
      return;
    }
    if(signal=="d" || signal=="d"){
      Serial.write("decrease speed");
      speedValue--;
      return;
    }
}

void polling()
{
    webSocketClient.sendData("ping");
}

bool connectToCenter(char host[], int port)
{
 if (client.connect("192.168.1.104", 3005)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    return false;
  }

  // Handshake with the server
  webSocketClient.path = "/";
  webSocketClient.host = "192.168.1.104";
  
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
    webSocketClient.sendData("iamrobot");
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

