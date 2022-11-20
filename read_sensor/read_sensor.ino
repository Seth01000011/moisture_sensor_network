/* notes

with Pin1 ADC-IN and Pin18 3.3V OUT
using just plain headers as "sensors"...

  damp napkin seems to register ~5,000 to ~6,500

  open air seems to read ~500 to ~1,500

with pin 11 ground and pin 12 ADC for battery readings
reading should be 1/10 of battery voltage 

*/

// threshold for touch wakeup
#define THRESHOLD 40

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
// using pin touchpin 2 - Also GPIO 2 - for wakeup detection
touch_pad_t touchpin; // enables touchpin

const int SLEEP_TIMER = 60000000; // 1 minute sleep time

void callback(void){}


void setup() {
  Serial.begin(115200);
  pinMode(ADC_SOIL_PWR, INPUT_PULLUP);
  pinMode(ADC_BATT_GND, OUTPUT);
  pinMode(LED, OUTPUT);


  //Setup interrupt on Touch Pad 2 (GPIO2)
  touchAttachInterrupt(T2, callback, THRESHOLD);
  Serial.println("Setup done");


  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  // enable timer wakeup
  esp_sleep_enable_timer_wakeup(SLEEP_TIMER);

}
// the loop function runs over and over again forever
void loop() {
  int moisture_int = 0;
  float moisture_float = 0;
  for (int i = 0; i < 20; i++) {

    digitalWrite(LED, HIGH);
    digitalWrite(ADC_BATT_GND, HIGH);
    // Serial.println("LED is on, and ADC_BATT_GND (11) is HIGH");


    int moisture = 0;
    delay(100);
    moisture = analogRead(1);
    moisture_int = moisture + moisture_int;
    delay(100);
    // Serial.printf("The current reading for moisture is %d\n", moisture);


    delay(100);
    digitalWrite(LED, LOW);
    digitalWrite(ADC_BATT_GND, LOW);
    // Serial.println("LED is OFF, and ADC_BATT_GND (11) is LOW");
    delay(100);
  }
  float moisture_average = float(moisture_int)/20;
  Serial.println("Moisture average is " + String(int(round(moisture_average))));
}
