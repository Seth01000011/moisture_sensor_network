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


// Replace with your network credentials
// const char* ssid     = "REPLACE_WITH_YOUR_SSID";
// const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// REPLACE with your Domain name and URL path or IP address with path
// const char* serverName = "http://192.168.0.3/post-esp-data.php";
const char* serverName = "http://192.168.0.3/get-request.txt";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
// String apiKeyValue = "";

String sensorName = "TempSensor1";
String sensorLocation = "Crawlspace";

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

// #define SEALEVELPRESSURE_HPA (1013.25)

// Adafruit_BME280 bme;  // I2C
//Adafruit_BME280 bme(BME_CS);  // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);  // software SPI

void setup() {
  Serial.begin(115200);
  delay(5000);
  
  Serial.println("Connecting to " + String(ssid) + " with pass " + String(pass));
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // (you can also pass in a Wire library object like &Wire2)
  // bool status = bme.begin(0x76);
  // if (!status) {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring or change I2C address!");
  //   while (1);
  // }
}

void loop() {
  //Check WiFi connection status
  while(!WiFi.isConnected() ){
    delay(5000);
    Serial.println("Reconnecting...");
    WiFi.reconnect();
  };
  Serial.println("Calling WiFi.status()==WL_CONNECTED");
  if(WiFi.status()== WL_CONNECTED){
    Serial.println("WiFi is connected");
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    // http.begin(client, serverName);
    Serial.println("Sending http.begin(client, serverName)");
    http.begin(String(serverName));

    // Specify content-type header
    // Serial.println("http.addHeader");
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // http.addHeader("Content-Type", "text/plain");
    
    // Prepare your HTTP POST request data
    // String httpRequestData = "api_key=" + String(apiKeyValue) + "&sensor=" + sensorName
    //                       + "&location=" + sensorLocation + "&value1=" + String(970)
    //                       + "&value2=" + String(65.3) + "";
    // Serial.print("httpRequestData: ");
    // Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    // Serial.println("Creating httpRequestData");
    // Serial.println("API key is " + String(apiKeyValue));
    // String httpRequestData = "api_key="+String(apiKeyValue)+"&sensor=TempSensor1&location=CrawlSpace&value1=24.75&value2=49.54&value3=00.0";
    // Serial.print("httpRequestData: ");
    // Serial.println(httpRequestData);

    // // Send HTTP POST request
    // Serial.println("Sending http.POST(httpRequestData)");
    // int httpResponseCode = http.POST(httpRequestData);
     int httpResponseCode = http.GET();
    // If you need an HTTP request with a content type: text/plain
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("HTTP failed, Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  WiFi.disconnect();
  Serial.println("Disconnected wifi!");  
  delay(30000);
  
  
}
