#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsServer.h>

#define ledpin D2

const char *ssid = "Musila";  
const char *password = "012345678";
SoftwareSerial nodemcu(D6,D5);


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


//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.43.87:8000/api/getdata";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(9600); 
  nodemcu.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     IPAddress ip(192, 168, 1, 200);
  // IPAddress gateway(192, 168, 1, 1);
  // IPAddress subnet(255, 255, 255, 0);
  // WiFi.config(ip, gateway, subnet);
  // Serial.println(WiFi.localIP());

  // server.on("/", handleRoot);
  // server.on("/led", toggleLed);
  // server.on("/led/on", turnOnLed);
  // server.on("/led/off", turnOffLed);
  // server.begin();
  // Serial.println("HTTP server started");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 

  
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
    
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject&  data=jsonBuffer.parseObject(nodemcu);

//  Serial.write(Serial.read());
//  String mydata=Serial.read();
//  Serial.println(mydata);



  if(data == JsonObject::invalid()){
    // Serial.println("Invalid Json object");
   
    jsonBuffer.clear();
   
    return;
    
  }

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

   



   String waterValue, photocell, postData,temp,photo,moisture;
   waterValue=String(waterLevel);
   temp=String(temperature);
   photo=String(photocellValue);
   moisture=String(soilMoistureValue);
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "?temperature=24.37";
      
      // Your Domain name with URL path or IP address with path
      // http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request

         postData="waterlevel=" + waterValue  + "&photocell=" + photo  + "&soilmoisture=" + moisture  + "&temperature="  + temp;
  //  postData="waterlevel=" + waterValue;
 
   http.begin(client,serverName);
   http.addHeader("Content-Type","application/x-www-form-urlencoded");

    
      int httpResponseCode = http.POST(postData);
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        Serial.println("---------------------------------------------------");
        delay(6000);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
         Serial.println("---------------------------------------------------");
        delay(6000);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

// const char *host = "http://192.168.43.87:8000/api/getdata";

// SoftwareSerial nodemcu(D6,D5);

// WiFiClient client;



// boolean ledState = false;

// ESP8266WebServer server(80);

// void handleRoot() {
//   // Sending sample message if you try to open configured IP Address
//   server.send(200, "text/html", "<h1>You are connected</h1>");
// }



// void toggleLed() {
//   ledState = ! ledState;
//   if (ledState == true) {
//     digitalWrite(ledpin, ledState);
//     server.send(200, "text/plain", "Off");
//     Serial.println("LED Off");
//   } else {
//     digitalWrite(ledpin, ledState);
//     server.send(200, "text/plain", "On");
//     Serial.println("LED On");
//   }
// }
// void turnOnLed() {
//   ledState = true;
//   digitalWrite(ledpin, ledState);
//   server.send(200, "text/plain", "On");
//   Serial.println("LED On");
// }

// void turnOffLed() {
//   ledState = false;
//   digitalWrite(ledpin, ledState);
//   server.send(200, "text/plain", "Off");
//   Serial.println("LED Off");
// }

// void setup() {
//   pinMode(ledpin, OUTPUT);
//  Serial.begin(9600);
//  nodemcu.begin(9600);

//   while(!Serial) continue;
   

//    WiFi.mode(WIFI_OFF);  
//    delay(1000);
//   WiFi.mode(WIFI_STA);

//    WiFi.begin(ssid, password);     //Connect to your WiFi router
//   Serial.println("");
  
//   Serial.print("Connecting");
//   // Wait for connection
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");



//   IPAddress ip(192, 168, 1, 200);
//   IPAddress gateway(192, 168, 1, 1);
//   IPAddress subnet(255, 255, 255, 0);
//   WiFi.config(ip, gateway, subnet);
//   Serial.println(WiFi.localIP());

//   server.on("/", handleRoot);
//   server.on("/led", toggleLed);
//   server.on("/led/on", turnOnLed);
//   server.on("/led/off", turnOffLed);
//   server.begin();
//   Serial.println("HTTP server started");

    
//   }

//   Serial.println("");
//   Serial.print("Connected to ");
//   Serial.println(ssid);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP()); 
// }


// void loop() {

//   server.handleClient();
  
//   StaticJsonBuffer<1000> jsonBuffer;
//   JsonObject&  data=jsonBuffer.parseObject(nodemcu);

// //  Serial.write(Serial.read());
// //  String mydata=Serial.read();
// //  Serial.println(mydata);



//   if(data == JsonObject::invalid()){
//     // Serial.println("Invalid Json object");
   
//     jsonBuffer.clear();
   
//     return;
    
//   }

//   Serial.println("Json Data received");
//   Serial.print("Recived water level: ");
//   int waterLevel=data["waterLevel"];
//   Serial.println(waterLevel);
//   Serial.print("Received photocell value: ");
//   int photocellValue=data["photocellvalue"];
//   Serial.println(photocellValue);
//   Serial.print("Received soil moisture value: ");
//   int soilMoistureValue=data["soilMoisture"];
//   Serial.println(soilMoistureValue);
//   float temperature=data["temperature"];
//   Serial.print("Temperature: ");
//   Serial.println(temperature);

   



//    String waterValue, photocell, postData,temp,photo,moisture;
//    waterValue=String(waterLevel);
//    temp=String(temperature);
//    photo=String(photocellValue);
//    moisture=String(soilMoistureValue);

  
//   //  postData="waterlevel=" + waterValue  + "&photocell=" + photo  + "&soilmoisture=" + moisture  + "&temperature="  + temp;
//    postData="waterlevel=" + waterValue;
//   HTTPClient http;
//    http.begin(client,"http://192.168.43.87:8000/api/getdata");
//    http.addHeader("Content-Type","application/x-www-form-urlencoded");

//     int code=http.GET();
//     Serial.println(code);
//   //  int httpCode = http.POST(postData); 
//    int httpCode = http.POST("waterlevel=1337");
//    String payload = http.getString();
//   //  Serial.println(ht);

//    Serial.println(httpCode);
//    Serial.println(payload);

//   //  if(httpCode  != 200){
//   //   Serial.println(http.connected);
//   //  }


//     http.end();
//     Serial.println("--------------------------------------------------------");

//     delay(6000); 





//   // }

// // }