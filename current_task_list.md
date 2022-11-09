# Completed Tasks/TODO/Notes

## Initial test of moisture sensor microcontrollers (MCUs)

- Capacitive sensors are knock-off junk - Do not function properly. Will use headers
- 18650 battery sled seems to work properly.
- Unable to get voltage divider to work, or switch mode supply to work in a low-power
  mode. Will rely on power consumption estimation and failure of MCUs
  to report as an indication of failure.
- Code seems to work properly using plain headers off of microcontrollers to detect
  moisture.
- Communication between many ESP(8266/32) to one ESP (32) working right now.

## Initial setup of server

(See task list in server_setup.md)

- Server is set up.
  - LAMP stack
    - Fedora
    - Apache
    - MySQL (mariaDB)
    - PHP
  - Tailscale for remote access
- ESP GET and POST requests using HTTPClient is reporting access denied(?)
- test POST request through terminal or other program on computer instead of MCU
1. Verify POST works from terminal (server issue or MCU issue?)

2a. If server -
    - look into other server technologies
    - replace PHP with Python (Flask)
    - test again

2b. If MCU -
    - build a better HTTP request


# TODO

## Server

### left off on randomnerdtutorials esp8266 lamp server step 5 (esp32 code)

https://randomnerdtutorials.com/esp32-esp8266-raspberry-pi-lamp-server/

- Still needs code added to analyze or intercept HTTP POST requests
  https://www.esp8266.com/viewtopic.php?f=32&t=4840

  use POST example from
  https://esp8266-shop.com/blog/how-to-http-get-and-post-requests-with-esp8266/

- Still needs code to send data when readings abnormal (Python?)
- Need to ensure database is setup properly
- SSD temperature readout seems high. Check whether this is
  an inaccurate reading, or if laptop needs cooling system cleaned

## MCUs

- HTTPClient POST request is failing. Continue to investigate. See initial set up
of server troubleshooting steps
- Need to modify code for MCUs. Communication works properly, but need to insert
  code for reading analog input from header pins
