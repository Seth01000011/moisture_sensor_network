/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

/*

THIS IS WORKING AS OF RIGHT NOW
20221204 
Using native espressif libraries on receiving end to ensure channel 1 is selected
for receiver

*/

#include <esp_now.h>
#include <WiFi.h>
#include "temp_sensor_driver.h"
#include <esp_wifi.h>

#define HIGHEST_CHANNEL 12
#define LOWEST_CHANNEL 1

int wifi_channel = 1;

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x84, 0xF7, 0x03, 0xF4, 0xE0, 0x94};
#define BOARD_ID 1

// threshold for touch wakeup
#define THRESHOLD 40
touch_pad_t touchPin;

// Timer for sleep
#define TIME_TO_SLEEP 10800000000 // 10800000000 us -> 3 hours per sleep cycle
const int LED = 15;

// pin assignments
const int ADC_SOIL_IN = 1;
const int ADC_SOIL_PWR = 18;
// using touchpin2 - AKA GPIO 2 - for wakeup detection


// using pin touchpin 2 - Also GPIO 2 - for wakeup detection
touch_pad_t touchpin; // enables touchpin


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  int id; // must be unique for each sender board
  int x;
  int y;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

void WiFiReset() {
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(100);
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("The status of the delivery is " + String(status));
}

void callback() {
  pinMode(ADC_SOIL_PWR, INPUT_PULLUP);
}

void setup()
{

  pinMode(LED, OUTPUT);    
  digitalWrite(LED, HIGH);

  WiFiReset();


  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();
  delay(1000);

  // enable timer wakeup
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
  delay(1000);
  
  // turn on timer wakeup
  Serial.println("Enabling sleep timer for " + String(TIME_TO_SLEEP));
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP); // 60 seconds for sleep
  
  //Setup interrupt on Touch Pad 2 (GPIO2)
  touchAttachInterrupt(T2, callback, THRESHOLD);
  delay(1000);

  // Init Serial Monitor
  Serial.begin(115200);
  delay(1000);


  Serial.println("I woke up!!!");



  digitalWrite(LED, LOW);
  delay(100);


  pinMode(ADC_SOIL_PWR, INPUT_PULLUP);
  delay(1000);

  int moisture_int = 0;
  float moisture_float = 0;
  for (int i = 0; i < 20; i++) {

    digitalWrite(LED, HIGH);
    // Serial.println("LED is on, and ADC_BATT_GND (11) is HIGH");


    int moisture = 0;
    delay(100);
    moisture = analogRead(1);
    moisture_int = moisture + moisture_int;
    delay(100);
    // Serial.printf("The current reading for moisture is %d\n", moisture);


    delay(100);
    digitalWrite(LED, LOW);
    // Serial.println("LED is OFF, and ADC_BATT_GND (11) is LOW");
    delay(100);
  }
  float moisture_average = float(moisture_int)/20;
  Serial.println("Moisture average is " + String(int(round(moisture_average))));
  
  // // get temp of MCU

  // Initialize touch pad peripheral, it will start a timer to run a filter
  Serial.println("Initializing Temperature sensor");
  float tsens_out;
  temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
  temp_sensor_get_config(&temp_sensor);
  temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
  temp_sensor_set_config(temp_sensor);
  temp_sensor_start();
  Serial.println("Temperature sensor started");
  vTaskDelay(1000 / portTICK_RATE_MS);
  temp_sensor_read_celsius(&tsens_out);
  Serial.println("Temperature out celsius " + String(int(round(tsens_out))));  


  pinMode(LED, OUTPUT);    
  digitalWrite(LED, HIGH);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);


  // WiFi.channel(6);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
  }
  Serial.println("Added peer!");

  Serial.println("Board number is " + String(BOARD_ID));            
  // Set values to send
  myData.id = BOARD_ID;
  myData.x = int(round(tsens_out));
  myData.y = int(round(moisture_average));

  // check channel that it is broadcasting on
  Serial.println("Channel is " + String(peerInfo.channel));

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  delay(100);
  Serial.println(result);

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }

  digitalWrite(LED, LOW);
  delay(100);


  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");

  
}

void loop(){

}
