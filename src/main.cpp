#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsServer.h>

#define ledpin D2

const char *ssid = "Intellijay";  
const char *password = "1234567890";


const char *host = "http://192.168.1.100:8000/api/getdata";

SoftwareSerial nodemcu(D6,D5);


WiFiClient client;
HTTPClient http;

boolean ledState = false;

ESP8266WebServer server(80);

void handleRoot() {
  // Sending sample message if you try to open configured IP Address
  server.send(200, "text/html", "<h1>You are connected</h1>");
}



void toggleLed() {
  ledState = ! ledState;
  if (ledState == true) {
    digitalWrite(ledpin, ledState);
    server.send(200, "text/plain", "Off");
    Serial.println("LED Off");
  } else {
    digitalWrite(ledpin, ledState);
    server.send(200, "text/plain", "On");
    Serial.println("LED On");
  }
}
void turnOnLed() {
  ledState = true;
  digitalWrite(ledpin, ledState);
  server.send(200, "text/plain", "On");
  Serial.println("LED On");
}

void turnOffLed() {
  ledState = false;
  digitalWrite(ledpin, ledState);
  server.send(200, "text/plain", "Off");
  Serial.println("LED Off");
}

void setup() {
  pinMode(ledpin, OUTPUT);
 Serial.begin(9600);
 nodemcu.begin(9600);

  while(!Serial) continue;
   

   WiFi.mode(WIFI_OFF);  
   delay(1000);
  WiFi.mode(WIFI_STA);

   WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");



  IPAddress ip(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led", toggleLed);
  server.on("/led/on", turnOnLed);
  server.on("/led/off", turnOffLed);
  server.begin();
  Serial.println("HTTP server started");

    
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
}


void loop() {

  server.handleClient();
  
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject&  data=jsonBuffer.parseObject(nodemcu);

 



  if(data == JsonObject::invalid()){
    // Serial.println("Invalid Json object");
   
    jsonBuffer.clear();
   
    return;
    
  }else{

  Serial.println("Json Data received");
  Serial.print("Recived water level: ");
  int waterLevel=data["waterLevel"];
  Serial.println(waterLevel);
  Serial.print("Received photocell value: ");
  int photocellValue=data["photocellvalue"];
  Serial.println(photocellValue);
  Serial.print("Received soil moisture value: ");
  int soilMoistureValue=data["soilMoisture"];
  Serial.println(soilMoistureValue);
  float temperature=data["temperature"];
  Serial.print("Temperature: ");
  Serial.println(temperature);




  Serial.println("hell");
  


  //  HTTPClient http;

   String waterValue, photocell, postData,temp;
   waterValue=String(waterLevel);
   temp=String(temperature);

   postData="waterlevel=" + waterValue  + "&photocell=" + photocellValue + "&soilmoisture=" + soilMoistureValue + "&temperature="  + temp;
   http.begin(client,"http://192.168.1.100:8000/api/getdata");
   http.addHeader("Content-Type","application/x-www-form-urlencoded");


   int httpCode = http.POST(postData); 
   String payload = http.getString();

   Serial.println(httpCode);
  //  Serial.println(payload);


    http.end();
    Serial.println("--------------------------------------------------------");

    delay(6000); 





  // }
}
}