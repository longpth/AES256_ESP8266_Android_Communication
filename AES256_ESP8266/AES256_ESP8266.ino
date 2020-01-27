#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "utilsP4F.h"

#define DEBUG

#define LED_OUT LED_BUILTIN

const char* ssid = "xxxxxxx"; //your wifi ssid
const char* password = "yyyyyyyyyy";  //your wifi password 
const char* ssid_ap = "ESP8266";
const char* password_ap = "esp8266Testing";

int ledState = LOW;

unsigned long previousMillis = 0;
const long interval = 1000;

const int PACKAGE_SIZE = 512;
byte packetBuffer[PACKAGE_SIZE];

//Creating UDP Listener Object. 
WiFiUDP UDPServer;
unsigned int UDPPort = 6868;
unsigned int clientPort = 6969;

std::string AESPass = "12345678";

void testAES(){
  std::string test = "I am an engineer and I like engineering";
  std::string passWord = "12345678";

  std::string encryptedStr = P4F::encrypt(test, passWord);
  std::string decryptedStr = P4F::decrypt(encryptedStr, passWord);

  String tmp = String(encryptedStr.c_str());
  String tmp2 = String(decryptedStr.c_str());
  Serial.println(tmp);
  Serial.println(tmp2);


}

void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.println();
  Serial.println();

#if 0
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#else
  WiFi.softAP(ssid_ap, password_ap);
  Serial.println("");

  Serial.println();
  Serial.print("Server IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Server MAC address: ");
  Serial.println(WiFi.softAPmacAddress());
#endif

  UDPServer.begin(UDPPort); 

  memset(packetBuffer, 0, PACKAGE_SIZE);

  Serial.println("Server Start\n");

  testAES();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(LED_OUT, ledState);
  }
  processUDPData();
  // String strSerial = processSerialData();
  // if(strSerial!=""){
  //   Serial.println(strSerial);
  // }
}

String processSerialData(){
  String content = "";
  while (Serial.available()) {
      content = Serial.readStringUntil('\r');
  }
  return content;
}

void sendEncryptedMessageUDP(String message, IPAddress addr, int clientPort){
  UDPServer.beginPacket(addr, clientPort);
  std::string encryptedMessage = P4F::encrypt(message.c_str(), AESPass);
  UDPServer.write(encryptedMessage.c_str());
  UDPServer.endPacket();
}

void processUDPData(){
  int cb = UDPServer.parsePacket();
  if (cb) {
    UDPServer.read(packetBuffer, PACKAGE_SIZE);
    // UDPServer.flush();
    IPAddress addr = UDPServer.remoteIP();
    std::string messageRecv = std::string((char*)packetBuffer);
    std::string decryptedMsg = P4F::decrypt(messageRecv, AESPass);
    String message = String(decryptedMsg.c_str());
#ifdef DEBUG
    Serial.print("receive: ");
    Serial.print(String(messageRecv.c_str()));
    Serial.print("decrypted message: ");
    Serial.println(message);
    Serial.print("from: ");
    Serial.println(addr);
#endif
    String response = "I am ESP8266, I got: " + message;
    sendEncryptedMessageUDP(response, addr, clientPort);
    sendEncryptedMessageUDP(response, addr, clientPort);
    memset(packetBuffer, 0, PACKAGE_SIZE);
  }
}
