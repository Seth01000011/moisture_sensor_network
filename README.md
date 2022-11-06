# Moisture Sensor Network

## Project goals:

1. Read whether or not moisture is present
2. Send that value to server
3. Notify family as to presence of moisture

### Secondary goals

1. Send temperature data as well
2. Send battery level data as well

### Tertiary goals

1. Enable ability to update program over Bluetooth Low Energy (BLE) or WIFI

# Implementation details

### Sensors and microcontrollers

#### Communication

- Use BLE mesh network to communicate across crawlspace
  - Docs:
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp-ble-mesh.html
  - Getting started:
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-ble-mesh/ble-mesh-index.html#getting-started-with-ble-mesh
- Unique (easy-to-read) identifier for each device
- Label the devices physically
- Use one microcontroller as BLE server for all BLE devices, to gather
  and send data to LAMP (Linux, Apache, MySQL, PHP/Python) stack computer
  server over wifi

#### Sensor data

- Trigger communication on 6 hour interval OR if moisture is detected
- Maintain low power consumption at most times
- Use ULP (Ultra-Low-Power) processor to wake device
  - on moisture > xxxx
  - on timer > 6 hours
  - connect via BLE and send moisture (and temperature)
- average out and/or smooth sensor readings

### Server

#### Receiving and storing data

- LAMP stack
- Tailscale for remote access/troubleshooting
  https://tailscale.com/
- Script to check over values (OR) REST API to trigger alerts
  - Learn more about REST API in order to utilize it to fullest potential

#### Sending data alerts

- Use Twilio or Gmail to notify family as to alert status
- Alert when interval > 7 hours for any one sensor
- Alert when value == Moisture present & include sensor ID and location

### Physical Considerations

- Waterproof microcontrollers
- Ensure dust/dirt/moisture doesn't impair function
  - Current thoughts - small tupperware containers + conformal coating + hot glue or
    liquid tape
- Moisture detection implementation:
  - Currently using headers - punch through bottom of tupperware?
  - Waterproofing holes? Will it work sufficiently?
  - Add cloth to bottom of tupperware to wick moisture to headers
- Test typical power consumption of final program and estimate time to dead battery
