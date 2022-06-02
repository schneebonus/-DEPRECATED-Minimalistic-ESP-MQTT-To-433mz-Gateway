## Minimalistic MQTT to 433mz remote socket gateway for ESP8266

## DEPRECATED: Use https://github.com/schneebonus/ESP8266-MQTT-To-433mz-Gateway for a better version without hardcoded devices.

A robust and minimalistic mqtt to 433mhz gateway for the esp8266.
Enables cheap wireless sockets from the DIY store to be controlled via the MQTT home automation server.
This project follows a minimalist approach, designed solely for reliability.
Therefor there is no fancy hotspot, ui or webapp.

### Requirenments

- ESP8266 board definition ( https://arduino-esp8266.readthedocs.io/en/latest/installing.html )
- PubSubClient.h ( https://pubsubclient.knolleary.net/ )
- RCSwitch.h ( https://github.com/sui77/rc-switch )
- ESP8266WiFi.h ( https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi )
- Ticker.h ( https://github.com/esp8266/Arduino/tree/master/libraries/Ticker )

### Install

- set wifi settings (ssid and passsword)
- set mqtt settings (mqttServer, mqttPort, mqttUser, mqttPassword)
- set 433mhz devices (deviceCount, devices)
- deploy to your esp8266

## Update devices

Just update the configuration and re-deyploy the project to your esp8266:
- set 433mhz devices (deviceCount, devices)
- deploy to your esp8266

There is no webapp or api.


### Background information

- 433mhz sender is expected to be on port 9 of the esp8266
- main loop has to finish every cycle in 10 seconds or the watchdog restarts the device
- if wifi reconnect fails 5 times in a row, the revice restarts
- there are no debugging outputs or serial prints

### Further Improvements:

- MQTT over TLS ( + verify certificate fingerprint )
- port project to esp32 and use firmware encryption to protect the passwords
- publish sensor values to mqtt channels
- publish "i am alive" to mqtt channels
- lite version without 433mhz for sonoff remotes
