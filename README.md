# Backstory

A pipe burst in my parent's crawlspace this October. Had my Dad not been in there on a
whim, to replace the bait in some mouse traps, it would have gone unnoticed until at
least April of next year.

While the leak was small, it was enough to have caused some
mold/mildew buildup in the insulation above the leak. It had leaked at least 20 gallons
at the point it was found, pooling on top of the vapor barrier my brother and I had
installed the prior spring.

To reduce the chances of this going undetected in the future, I have decided to use
my skills in programming microcontrollers, setting up local servers, and creating
scripts/programs to interact with API's and send notifications when something has gone
awry.

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

- Will use ESP-NOW to communicate between devices
  https://www.espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf

  Note on terminology:
  It seems the parent/child (master/slave) is backwards in my implementation. The
  majority of microcontrollers will be sending data to one single one, which interfaces
  with the local wifi and communicates with the server. The examples provided in
  the ESP/Arduino library use the parent to send data to the children.

  I will be using the multitude of parents to send data to one child - Which interfaces
  with the local server.

- Unique (easy-to-read) identifier for each device
- Label the devices physically
- Use one microcontroller as ESP-NOW parent for all ESP-NOW child devices, to gather
  and send data to LAMP (Linux, Apache, Django's SQLite, Python (Django)) server
  server over wifi

#### Sensor data

- Trigger communication on 6 hour interval OR if moisture is detected
- Maintain low power consumption at most times
- Use ULP (Ultra-Low-Power) processor to wake device
  - on moisture > xxxx
  - on timer > 6 hours
  - connect via ESP-NOW and send moisture (and temperature)
- average out and/or smooth sensor readings

### Server

#### Receiving and storing data

- LASD (Linux, Apache, SQLite, Django) stack
- Tailscale for remote access/troubleshooting
  https://tailscale.com/
- REST API to trigger alerts
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
- Utilize 18650 batteries with ~3,000 mAh rating

# Removed from project

- Use BLE mesh network to communicate across crawlspace
- Docs:
  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp-ble-mesh.html
- Getting started:
  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-ble-mesh/ble-mesh-index.html#getting-started-with-ble-mesh

- BLE Mesh is... advanced.

- LAMP Stack. Swapped to LAS(QLite)P(ython(Django))
