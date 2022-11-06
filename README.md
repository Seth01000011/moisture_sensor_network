# Moisture Sensor Network

## Project goals:

1. Read whether or not moisture is present
2. Send that value to server
3. Notify family as to presence of moisture

### Secondary goals

1. Send temperature data as well
2. Send battery level data as well

### Tertiary goals

1. Enable ability to update program over BLE or WIFI

## Implementation details

### Sensors and microcontrollers

- Use BLE mesh network to communicate across crawlspace
- Unique (easy-to-read) identifier for each device
- Label the devices physically
- Use one microcontroller as BLE server for all BLE devices, to gather
  and send data to LAMP stack computer server over wifi
- Maintain low power consumption at most times
- Trigger communication on 6 hour interval OR if moisture is detected

### Server

- LAMP stack
- Use Twilio or Gmail to notify family as to alert status
- Alert when interval > 7 hours for any one sensor
- Alert when value == Moisture present
- Script to check over values (OR) REST API
  - Learn more about REST API in order to utilize it to fullest potential

### Physical Considerations

- Waterproof microcontrollers
- Ensure dust/dirt/moisture doesn't impair function
  - Current thoughts - small tupperware containers + conformal coating + hot glue or
    liquid tape
- Test typical power consumption of final program
