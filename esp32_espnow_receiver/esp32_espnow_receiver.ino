/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/

// Rewrite the connection code for connecting to WiFi and transmitting data using native
// espressif libraries
// https://github.com/espressif/esp-idf/tree/7869f4e151e3ea2a308b991fbb8e9baa4cec313c/examples/wifi/getting_started/station

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
#include <esp_wifi.h>
#include <esp_netif.h>
#include "./data/secrets.h"

// Replace with your network credentials

const char* serverName = "http://192.168.1.13/api/data/";
// 192.168.1.13 - reserved ip for server on parents network


#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

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
  delay(500);
  Serial.println("Datetime is " + String(rtc.getTime("%FT%T")));
  delay(500);
  boardsStruct[myData.id-1].datetime = rtc.getTime("%FT%T");
  delay(500);
  Serial.println();
}

void sendDataToServer() {
  /*

  NEED TO REWRITE THIS USING NATIVE ESPRESSIF LIBRARIES FOR PROPER FUNCTION (I think...)


  */

  // Check WiFi connection status
  // Placed the connect loop in the main void loop() so that the wifi connects and disconnects
  // as needed throughout the day... 
  digitalWrite(LED, HIGH);

  // swap below with native esp32 c++ library 

  ESP_ERROR_CHECK( esp_netif_init());
  ESP_ERROR_CHECK( esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_start());

  // connect to wifi
  ESP_ERROR_CHECK( esp_wifi_connect());

  // uint8_t* currentChannel;
  // esp_wifi_get_channel(currentChannel, );
  // Serial.println("Wifi channel is " + String(currentChannel));
  WiFi.mode(WIFI_STA);
  
  Serial.println("Connecting to " + String(ssid) + " with pass " + String(pass));
  
  // esp_wifi_init();
  WiFi.begin(ssid, pass);
  delay(100);
  // Serial.println("Wifi channel is " + String(WiFi.channel()));
  // WiFi.channel();
  // esp_wifi_set_channel(0);")
  Serial.println("Connecting...");
  while(WIFI_EVENT_STA_DISCONNECTED) {
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
  // Serial.println("Wifi channel is " + String(WiFi.channel()));
  digitalWrite(LED, LOW);
  Serial.println(WiFi.localIP());


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
  WiFi.disconnect(true, true);
  esp_wifi_stop();

}

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

  // WiFi.mode(WIFI_STA);  // initialize wifi station mode
  // the following pulled from espressif's espnow example at
  // https://github.com/espressif/esp-idf/blob/7869f4e151e3ea2a308b991fbb8e9baa4cec313c/examples/wifi/espnow/main/espnow_example_main.c
  ESP_ERROR_CHECK( esp_netif_init());
  ESP_ERROR_CHECK( esp_event_loop_create_default());
  // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  wifi_config_t wifi_config;
  wifi_config.sta.ssid = "";
  wifi_config.sta.password = "";
          /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
          * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
          * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
    * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
          */
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WEP;
  wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
  

  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_start());
  ESP_ERROR_CHECK( esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

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
  delay(500);
  
  digitalWrite(LED, LOW);
  
  // configure time for ntp grab
  configTime(-18000, 0, ntpServer);

}


void loop() {
  // delay(10000);
  // Serial.println("Waking up!");
  Serial.println("Looping...");

  Serial.println("Setting wifi channel to 1 to accept ESPNOW messages...");
  ESP_ERROR_CHECK( esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

  uint8_t currentChannel;
  esp_wifi_get_channel(currentChannel, &WIFI_SECOND_CHAN_NONE);
  Serial.println("Wifi channel is " + String(currentChannel));
  Serial.println("CPU Freq is " + String(getCpuFrequencyMhz()));

  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  // Serial.println("Wifi channel is now " + String(WiFi.channel()));

  sendDataToServer();
  delay(SEND_TO_SERVER_INTERVAL);
}
