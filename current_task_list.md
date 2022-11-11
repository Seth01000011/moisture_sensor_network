# Completed Tasks/TODO/Notes

## Initial test of moisture sensor microcontrollers (MCUs)

- Capacitive sensors are knock-off junk - Do not function properly. Will use headers
- 18650 battery sled seems to work properly.
- Unable to get voltage divider to work, or switch mode supply to work in a low-power
  mode. Will rely on power consumption estimation and failure of MCUs
  to report as an indication of failure.
- Code seems to work properly using plain headers off of microcontrollers to detect
  moisture.

## Initial setup of server

(See task list in server_setup.md)

- Server is set up.
  - LAMP stack
    - Fedora
    - Apache
    - Django's built-in SQL database
    - Python
      - Django
  - Tailscale for remote access

# TODO

## Server

### Currently working through Django tutorial to get a handle.

### Then building API with Django to send data

- Need to create Django backend to interact with using sensors
  Rest API?

- Still needs code to send data when readings abnormal (Python?)

- Need to ensure database is setup properly

- SSD temperature readout seems high. Check whether this is
  an inaccurate reading, or if laptop needs cooling system cleaned

## MCUs

- Need to modify code for MCUs. Found what seems like good example to start
  with
