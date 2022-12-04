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
// #include "./data/secrets.h"
#include <esp_now.h>
#include <ESP32Time.h>

// Replace with your network credentials

const char* serverName = "http://192.168.1.13/api/data/";
// 192.168.1.13 - reserved ip for server on parents network


// // Time stuffs
const char* ntpServer = "pool.ntp.org";

//ESP32Time rtc;
ESP32Time rtc(0);  // offset in seconds GMT+1

// LED
const int LED = 15;
// button
const int BUTTON = 0;

// CPU Frequency setter...
const int freq = 80;

// delay used to allow initial data to be sent from MCUs  
// const long STARTUP_DELAY = 300000; // 5 minutes in mi  lliseconds - 
const int US_IN_MS = 1000; // # of microseconds in 1 millisecond
const int MS_IN_MINUTE = 60000; // ms in one minute     
const int MS_IN_HOUR = 3600000; // ms in one hour
const long STARTUP_DELAY = MS_IN_MINUTE * 5; // 5 minutes

// interrupter to send data
bool DELAY_BOOLEAN = true;

// delay between main loops to send data every 6 hours to server
// const long SEND_TO_SERVER_INTERVAL = (43200000 - STARTUP_DELAY); // 6 hours - startup delay in milliseconds
// const long DELAY_LOOP_INTERVAL = 100;
const long SEND_TO_SERVER_INTERVAL = ((MS_IN_MINUTE*10));// - (STARTUP_DELAY)); // 1 hour (in ms) minus start delay
// const long SEND_TO_SERVER_INTERVAL = ((MS_IN_MINUTE)*5*(US_IN_MS)); // delay for sleep, 5 minutes
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

RTC_DATA_ATTR struct_message boardsStruct[6] {mcu1, mcu2, mcu3, mcu4, mcu5, mcu6}; // array to hold all the message from senders

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
  delay(5000);

  // lower clock speed to reduce power consumption and heat
  // lolin s2 mini has 40MHz crystal

  Serial.println("Setting frequency to " + String(freq) + " MHz...");
  delay(100);
  setCpuFrequencyMhz(freq);
  delay(500);
  Serial.println("Set cpu freq to " + String(getCpuFrequencyMhz()));
  delay(500);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);    
  digitalWrite(LED, HIGH);

  WiFi.mode(WIFI_STA);  // initialize wifi station mode
  // WiFi.mode(WIFI_AP_STA);

  delay(100);
  digitalWrite(LED, LOW);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error intializing ESP_NOW");
    return;
  }
  delay(100);
  digitalWrite(LED, HIGH);

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info
  esp_now_register_recv_cb(OnDataRecv);
  
  digitalWrite(LED, LOW);
  
  // configure time for ntp grab
  configTime(-18000, 0, ntpServer);

  // esp_sleep_enable_timer_wakeup(SEND_TO_SERVER_INTERVAL);
  // esp_sleep_enable_wifi_wakeup();
  // WiFi.setSleep(false);

}

void loop() {
  // delay(10000);
  // Serial.println("Waking up!");
  Serial.println("Looping...");

  
  // Check WiFi connection status
  // Placed the connect loop in the main void loop() so that the wifi connects and disconnects
  // as needed throughout the day... 
  if(!WiFi.isConnected() ){
    digitalWrite(LED, HIGH);
    Serial.println("Connecting to " + String(ssid) + " with pass " + String(pass));
    // esp_wifi_init();
    WiFi.begin(ssid, pass);
    delay(100);
    Serial.println("Wifi channel is " + String(WiFi.channel()));
    WiFi.channel();
    // esp_wifi_set_channel(0);
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
    Serial.println("Mac address is " + String(WiFi.macAddress()));
    delay(100);
    Serial.println("Wifi channel is " + String(WiFi.channel()));
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
    delay(2000);

    for (int j = 0; j < NUM_BOARDS; j++) {
      if (boardsStruct[j].x == 0) {
        Serial.println("Skipping " + String(j) + " cuz blank");
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
      Serial.println(String("{\"parent\":\"" + String(boardsStruct[j].id) 
      + "\",\"date_published\":\"" 
      + boardsStruct[j].datetime 
      + "\",\"mcu_temperature\":\"" + String(boardsStruct[j].x)
      + "\",\"moisture_reading\":\"" + String(boardsStruct[j].y) + "\"}"));

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
  }    
  else {
    Serial.println("WiFi Disconnected");
  }

  Serial.println("CPU Freq is " + String(getCpuFrequencyMhz()));

  WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
    Serial.println("Error intializing ESP_NOW");
    return;
  }
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  WiFi.mode(WIFI_AP);
  WiFi.begin();
  delay(100);
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info
  // WiFi.channel(1); // 20221204 trying this to get the esp now working 
  // WiFi.channel(1);
  esp_wifi_set_channel(1);
  Serial.println("Wifi channel is now " + String(WiFi.channel()));
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Disconnected wifi!");
  delay(SEND_TO_SERVER_INTERVAL);
}
