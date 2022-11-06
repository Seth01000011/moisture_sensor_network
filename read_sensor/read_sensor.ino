/* notes

with Pin1 ADC-IN and Pin18 3.3V OUT
using just plain headers as "sensors"...

  damp napkin seems to register ~5,000 to ~6,500

  open air seems to read ~500 to ~1,500

with pin 11 ground and pin 12 ADC for battery readings
reading should be 1/10 of battery voltage 

*/

//#define RGB_BRIGHTNESS 64 // Change white brightness (max 255)

// sets attentuation to default value (up to 3.3v)
// void analogSetAttenuation(adc_attenuation_t ADC_ATTEN_DB_11);

// the setup function runs once when you press reset or power the board
const int BATTERY_VOLTAGE_MULTIPLIER = 2; // voltage reading should be half of
// actual voltage in millivolts...  multiply by 2 and divide by 1000
const int LED = 15;
const int ADC_SOIL_IN = 1;
const int ADC_SOIL_PWR = 18;
const int ADC_BATT_GND = 11;
const int ADC_BATT_IN = 12;

void setup() {
  Serial.begin(115200);
  pinMode(ADC_SOIL_PWR, INPUT_PULLUP);
  pinMode(ADC_BATT_GND, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.println("Setup done");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);
  digitalWrite(ADC_BATT_GND, HIGH);
  Serial.println("LED is on, and ADC_BATT_GND (11) is HIGH");
  delay(5000);
  digitalWrite(LED, LOW);
  digitalWrite(ADC_BATT_GND, LOW);
  Serial.println("LED is OFF, and ADC_BATT_GND (11) is LOW");
  delay(5000);


  // int moisture = 0;
  // delay(5000);
  // moisture = analogRead(1);
  // delay(5000);
  // Serial.printf("The current reading for moisture is %d\n", moisture);
  // delay(5000);
  // int battery = 0;
  // delay(5000);
  // battery = analogReadMilliVolts(12);
  // delay(5000);
  // Serial.printf("The current battery millivoltage is %d\n", battery);
  // delay(5000);
}
