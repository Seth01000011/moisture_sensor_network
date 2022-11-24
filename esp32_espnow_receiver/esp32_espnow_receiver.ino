/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <Wire.h>
#include "./data/secrets.h"
#include <esp_now.h>
#include <ESP32Time.h>

// Replace with your network credentials
// const char* ssid     = "Replace with your ssid";
// const char* password = "Replace with your password";

const char* serverName = "http://192.168.0.30/espServer/api/data/";

// // Time stuffs
const char* ntpServer = "pool.ntp.org";

//ESP32Time rtc;
ESP32Time rtc(0);  // offset in seconds GMT+1

// LED
const int LED = 15;

// delay used to allow initial data to be sent from MCUs  
// const long STARTUP_DELAY = 300000; // 5 minutes in mi  lliseconds - 
const int MS_IN_MINUTE = 60000; // ms in one minute     
const int MS_IN_HOUR = 3600000; // ms in one hour
const long STARTUP_DELAY = MS_IN_MINUTE * 10; // 2 minutes

// delay between main loops to send data every 6 hours to server
// const long SEND_TO_SERVER_INTERVAL = (43200000 - STARTUP_DELAY); // 6 hours - startup delay in milliseconds
const long SEND_TO_SERVER_INTERVAL = ((MS_IN_HOUR*6) - (STARTUP_DELAY)); // 5 min (in ms) minus start delay

/************************
ESP_NOW stuffs
https://randomnerdtutorials.com/esp-now-many-to-one-esp32/
*************************/


// struct_message to hold data from each individual sending sensor MCU
typedef struct struct_message
{
  int id;
  int x; // mcu_temp reading
  int y; // moisture reading
  String datetime; // datetime stamp
} struct_message;


// Create a struct_message called myData
struct_message myData;

// initializing messages for each sender
int NUM_BOARDS = 6;

// initializing struct_messsages to (parentid), 0, 0 so that there is SOMETHING to print
struct_message mcu1 {1,0,0,"0"};
struct_message mcu2 {2,0,0,"0"};
struct_message mcu3 {3,0,0,"0"};
struct_message mcu4 {4,0,0,"0"};
struct_message mcu5 {5,0,0,"0"};
struct_message mcu6 {6,0,0,"0"};

struct_message boardsStruct[6] {mcu1, mcu2, mcu3, mcu4, mcu5, mcu6}; // array to hold all the message from senders

// callback func to be used when data is recv'd
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  // digitalWrite(LED, HIGH);
  char macStr[18];
  Serial.print("Packet received from: ");
  digitalWrite(LED, LOW);
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // digitalWrite(LED, HIGH);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // digitalWrite(LED, LOW);

  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].id = myData.id;
  boardsStruct[myData.id-1].x = myData.x;
  // digitalWrite(LED, HIGH);
  boardsStruct[myData.id-1].y = myData.y;
  Serial.printf("Board id: %d \n", boardsStruct[myData.id-1].id);
  Serial.printf("Temp is: %d \n", boardsStruct[myData.id-1].x);
  // digitalWrite(LED, LOW);
  Serial.printf("Moisture reading is: %d \n", boardsStruct[myData.id-1].y);
  Serial.println("Datetime is " + String(rtc.getTime("%FT%T")));
  boardsStruct[myData.id-1].datetime = rtc.getTime("%FT%T");
  Serial.println();
}

// get and print time

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED, OUTPUT);    
  digitalWrite(LED, HIGH);

  WiFi.mode(WIFI_STA);  // initialize wifi station mode

  digitalWrite(LED, LOW);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error intializing ESP_NOW");
    return;
  }

  digitalWrite(LED, HIGH);

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info
  esp_now_register_recv_cb(OnDataRecv);
  
  digitalWrite(LED, LOW);
  
  // configure time for ntp grab
  configTime(-18000, 0, ntpServer);

}

void loop() {
  Serial.println("Looping...");

  
  // Check WiFi connection status
  // Placed the connect loop in the main void loop() so that the wifi connects and disconnects
  // as needed throughout the day... 
  if(!WiFi.isConnected() ){
    digitalWrite(LED, HIGH);
    Serial.println("Connecting to " + String(ssid) + " with pass " + String(pass));
    WiFi.begin(ssid, pass);
    Serial.println("Connecting...");
    while(!WiFi.isConnected()) {
      digitalWrite(LED, LOW);
      delay(2000);
      digitalWrite(LED, HIGH);
      delay(2000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    digitalWrite(LED, LOW);
    Serial.println(WiFi.localIP());
  }

  Serial.println("Calling WiFi.status()==WL_CONNECTED");


  if(WiFi.isConnected()){

    Serial.println("WiFi is connected");
    Serial.println("Syncing rtc to ntp server");
    while(rtc.getTime("%Y").toInt() < 2000 ) {
      // // set notification call-back function
      delay(1000);
      digitalWrite(LED, HIGH);
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)){
        rtc.setTimeStruct(timeinfo);
      }
      delay(1000);
      digitalWrite(LED, LOW);
      Serial.println("Waiting for time to get set properly...");    
      Serial.println("Current datetime is " + rtc.getTimeDate());
    }
    digitalWrite(LED, LOW);

    Serial.println(rtc.getTime("%FT%T"));
    
    WiFiClient client;
    HTTPClient http;
    
    delay(STARTUP_DELAY);

    for (int j = 0; j < NUM_BOARDS; j++) {
      if (boardsStruct[j].x == 0) {
        continue;
      }
      digitalWrite(LED, HIGH);

      // Your Domain name with URL path or IP address with path
      // http.begin(client, serverName);
      Serial.println("Servername is " + String(serverName));
      http.begin(client, String(serverName));

      // Specify content-type header
      http.addHeader("Content-Type", "application/json");
      String httpRequestData = String("{\"parent\":\"" + String(boardsStruct[j].id) 
      + "\",\"date_published\":\"" 
      + boardsStruct[j].datetime 
      + "\",\"mcu_temperature\":\"" + String(boardsStruct[j].x)
      + "\",\"moisture_reading\":\"" + String(boardsStruct[j].y) + "\"}");

      // Send HTTP POST request
      Serial.println("Sending http.POST(httpRequestData)");
      int httpResponseCode = http.POST(httpRequestData);
      
      // leaving the GET line below for testing purposes
      // int httpResponseCode = http.GET(););
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        // String payload = http.getString();
        // Serial.println(payload);
      }
      else {
        Serial.print("HTTP failed, Error code: ");
        Serial.println(httpResponseCode);
      }

      // Free resources
      http.end();

      digitalWrite(LED, LOW);
      delay(500);
    }
    // to implement this, I need to go back to the django code and setup a warning if no message is received for a 
    // certain amount of time. Since these won't get sent at all... So for now, I'll let it continue to resend
    // the old values, since I have a warning set for that
    
    // struct_message mcu1 {1,0,0,"0"};
    // struct_message mcu2 {2,0,0,"0"};
    // struct_message mcu3 {3,0,0,"0"};
    // struct_message mcu4 {4,0,0,"0"};
    // struct_message mcu5 {5,0,0,"0"};
    // struct_message mcu6 {6,0,0,"0"};
  }    
  else {
    Serial.println("WiFi Disconnected");
  }

  WiFi.disconnect();
  Serial.println("Disconnected wifi!");
  delay(SEND_TO_SERVER_INTERVAL);

}
