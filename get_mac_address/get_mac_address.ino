// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/
// flash this program with Arduino IDE to an ESP to get its MAC address
// works for both 8266 and 32
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

void setup()
{
  Serial.begin(115200);
  Serial.println();
}

void loop()
{
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  delay(3000);
}
