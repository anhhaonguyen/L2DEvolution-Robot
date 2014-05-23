#include <SPI.h>
#include <WiFi.h>

char ssid[] = "LOZI"; 
char pass[] = "kjnhkHungvl1";
int keyIndex = 0;

int status = WL_IDLE_STATUS;
char server[] = "128.199.223.211";

WiFiClient client;

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
  if (connectToCenter(server, 3005)) {
    client.println("Hello sir!!!");
  }
}

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
    if(c=='s' || c=='S'){
      Serial.write("stop");
      analogWrite(3, 0);
      analogWrite(9, 0);
    }
    if(c=='f' || c=='F'){
      Serial.write("go forward");
      analogWrite(3, 16*speedValue);
      analogWrite(9, 0);
    }
    if(c=='b' || c=='B'){
      Serial.write("go backward");
      analogWrite(3, 0);
      analogWrite(9, 16*speedValue);
    }
    if(c=='r' || c=='R'){
      Serial.write("turn right");
      analogWrite(5, 128);
      analogWrite(6, 128);
    }
    if(c=='l' || c=='L'){
      Serial.write("turn left");
      analogWrite(5, 128);
      analogWrite(6, 0);
    }
    if(c=='s' || c=='S'){
      Serial.write("stop");
      analogWrite(3, 0);
      analogWrite(9, 0);
    }
    if(c=='u' || c=='U'){
      Serial.write("increase speed");
      speedValue++;
    }
    if(c=='d' || c=='D'){
      Serial.write("decrease speed");
      speedValue--;
    }
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    connectToCenter(server, 3005);
  }
}

bool connectToCenter(char host[], int port)
{
  if (client.connect(server, 3005)) {
    sendHandShake();
    return readHandShake();
  } 
  else {
    Serial.println("Connect fail :(");
    return false;
  } 
}

void poll()
{

}

void sendHandShake()
{
  Serial.println("send handshake to server");
  client.println("GET / HTTP/1.1");
  client.println("Host: 128.199.223.211/");
  client.println("Sec-WebSocket-Version: 13");
  client.println("Upgrade: websocket");
  client.println("Sec-WebSocket-Key: KCsYRvuDylR1T59hn5g/ig==");
  client.println("Connection: Upgrade");
  client.println("Origin: 128.199.223.211/");
  client.println();
}

bool readHandShake()
{
  Serial.println("read handshake to server");
  bool result = false;
  char character;
  String handshake = "", line;
  int maxAttempts = 300, attempts = 0;
  while(client.available() == 0 && attempts < maxAttempts) 
  { 
    delay(100); 
    attempts++;
  }
  while((line = readLine()) != "") {
    handshake += line + '\n';
  }
  Serial.println(handshake);
  result = handshake.indexOf("6XD14FnDL4E3zuGVM11QkbruQW8=") != -1;
  if(!result) {
         client.stop();
  }
  return true;
}

String readLine() {
  String line = "";
  char character;
  while(client.available() > 0 && (character = client.read()) != '\n') {
    if (character != '\r' && character != -1) {
      line += character;
    }
  }
  return line;
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


