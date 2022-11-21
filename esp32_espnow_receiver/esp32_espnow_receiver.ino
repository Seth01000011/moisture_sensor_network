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

// Replace with your network credentials
// const char* ssid     = "Replace with your ssid";
// const char* password = "Replace with your password";

// REPLACE with your Domain name and URL path or IP address with path
// const char* serverName = "http://192.168.0.3/post-esp-data.php"; http://server.local.com/espServer/api/data/
const char* serverName = "http://192.168.0.30/espServer/api/data/";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
// String apiKeyValue = "";

// String sensorName = "Arnold";
// String MCU_ID = "1";
// int MCU_TEMP = 999;
// int MOISTURE_READING = 999;


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
} struct_message;


// Create a struct_message called myData
struct_message myData;

// initializing messages for each sender
int NUM_BOARDS = 6;
// struct_message mcu1 { 1,  111,  111};
// struct_message mcu2 { 2,  222,  222};
// struct_message mcu3 { 3,  333,  333};


// initializing struct_messsages to (parentid), 0, 0 so that there is SOMETHING to print
struct_message mcu1 {1,0,0};
struct_message mcu2 {2,0,0};
struct_message mcu3 {3,0,0};
struct_message mcu4 {4,0,0};
struct_message mcu5 {5,0,0};
struct_message mcu6 {6,0,0};

struct_message boardsStruct[6] {mcu1, mcu2, mcu3, mcu4, mcu5, mcu6}; // array to hold all the message from senders

// callback func to be used when data is recv'd
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  /********************
  Insert code here to add timestamp to data received
  ALSO update the boardsStruct to contain a datetime stamp
  Then ALSO update the loop structure/api send thingy to include 
  this updated datetime stamp!

  Oh yeah, add (fast?) blinking LED to this section! That way I know when data 
  is received without watching serial terminal
  ********************/
  boardsStruct[myData.id-1].x = myData.x;
  boardsStruct[myData.id-1].y = myData.y;
  Serial.printf("Board id: %d \n", boardsStruct[myData.id-1]);
  Serial.printf("Temp is: %d \n", boardsStruct[myData.id-1].x);
  Serial.printf("Moisture reading is: %d \n", boardsStruct[myData.id-1].y);
  Serial.println();



}


void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);  // initialize wifi station mode

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error intializing ESP_NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // start by collecting data through esp_now from all of the sensors...

  /* 
    ESP NOW CODE
  */
  Serial.println("Looping...");
  // Access the variables for each board
  // for (int i = 0; i < NUM_BOARDS; i++) {
  //   int boardNum = boardsStruct[i].id;
  //   Serial.println(String(boardNum));
  //   int boardx = boardsStruct[i].x;
  //   Serial.println(String(boardx));
  //   int boardy = boardsStruct[i].y;
  //   Serial.println(String(boardy));
  // };
  // commenting out WiFi POST code to test ESP_NOW code


  //Check WiFi connection status
  // Placed the connect loop in the main void loop() so that the wifi connects and disconnects
  // as needed throughout the day...
  while(!WiFi.isConnected() ){
    delay(5000);
    
    Serial.println("Connecting to " + String(ssid) + " with pass " + String(pass));
    WiFi.begin(ssid, pass);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) { 
      delay(5000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  }
  Serial.println("Calling WiFi.status()==WL_CONNECTED");


  if(WiFi.status()== WL_CONNECTED){
    Serial.println("WiFi is connected");
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    // http.begin(client, serverName);
    Serial.println("Sending http.begin(client, serverName)");
    Serial.println("Servername is " + String(serverName));

    

    for (int j = 0; j < NUM_BOARDS; j++) {
      http.begin(client, String(serverName));

      // Specify content-type header
      Serial.println("http.addHeader");
      http.addHeader("Content-Type", "application/json");
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
      String httpRequestData = String("{\"parent\":\"" + String(boardsStruct[j].id) + "\",\"date_published\":\"1991-01-01T00:12:00-05:00\",\"mcu_temperature\":\"" + String(boardsStruct[j].x)
      + "\",\"moisture_reading\":\"" + String(boardsStruct[j].y) + "\"}");
      
      // Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
      // // Update the structures with the new incoming data
      // boardsStruct[myData.id-1].x = myData.x;
      // boardsStruct[myData.id-1].y = myData.y;
      // Serial.printf("Board id: %d \n", boardsStruct[myData.id-1]);
      // Serial.printf("Temp is: %d \n", boardsStruct[myData.id-1].x);
      // Serial.printf("Moisture reading is: %d \n", boardsStruct[myData.id-1].y);
      // Serial.println();


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
    }
  }    
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  WiFi.disconnect();
  Serial.println("Disconnected wifi!");  
  delay(60000);  

}
