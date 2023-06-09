# Project Information

## Module Information
Notes: ***Power connection not included, 5 V for modules and sensors***
### Sensors
#### DS18B20
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Digital|
|2|Pin connection|4|
|3|Total Sensor|7|
|4|Status|OK|
|5|Properties Sensing|Temperature|
|6|Unit|Celcius|
#### DHT22
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Digital|
|2|Pin Connection|8|
|3|Total Sensor|1|
|4|Status||
|5|Properties|Temperature & Humidity|
|6|Units|Celcius & RH (%)|
#### ZMCT101C
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Analog|
|2|Pin Connection|A7, A8, A9|
|3|Total Sensor|3|
|4|Status||
|5|Properties|Current|
|6|Units|mA|
#### ZMPT101
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Analog|
|2|Pin Connection|A6|
|3|Total Sensor|3|
|4|Status||
|5|Properties|Voltage|
|6|Units|Volts|
#### Pressure Transducer 1
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Analog|
|2|Pin Connection|A5|
|4|Status|Un-converted to pressure, from signal|
|5|Properties|Pressure (Supposedly)|
|6|Units|Bar|
#### Pressure Transducer 2
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Analog|
|2|Pin Connection|A4|
|4|Status|Un-converted to pressure, from signal|
|5|Properties|Pressure (Supposedly)|
|6|Units|Bar|
#### Pressure Transducer 3
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Analog|
|2|Pin Connection|A3|
|4|Status|Un-converted to pressure, from signal|
|5|Properties|Pressure (Supposedly)|
|6|Units|Bar|
### Modules
#### 6-Channel Relay
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Digital|
|2|Pin Connection|34, 35, 36, 37, 38, 39|
#### SD Card Module
|*No.*|*Arduino*|*Module*|
| ------------- | ------------- | ------------- |
|1|CS|53|
|2|MISO|50|
|3|MOSI|51|
|4|CLK|52|
#### RTC Module - DS1307
|*No.*|*Arduino*|*Module*|
| ------------- | ------------- | ------------- |
|1|SCL|21|
|2|SDA|20|
### Buzzer
|*No.*|*Information*|*Value*|
| ------------- | ------------- | ------------- |
|1|Signal Type|Digital|
|2|Pin Connection|5|
#### Nextion Display
|*No.*|*Arduino*|*Module*|
| ------------- | ------------- | ------------- |
|1|2 (RX)|TX|
|2|3 (TX)|RX|
#### ESP32
|*No.*|*Arduino*|*Module*|
| ------------- | ------------- | ------------- |
|1|12 (TX)|17 (RX)|
|2|13 (RX)|16 (TX)|
#### LED Indicator (3.3 V)
|*No.*|*Arduino*|*Indicating*|
| ------------- | ------------- | ------------- |
|1|41|DS18B20|
|2|42|DHT22|
|3|43|ZMCT101C|
|4|44|ZMPT101B|
|5|45|SD Card Module|
|6|46|RTC Module|
|7|47|ESP32 Board|
|8|48|5V Buzzer|
|9|49|Relay Module|

## ToDos
### Module & Sensor Testing
- [X] DS18B20
- [X] ZMCT101C
- [X] ZMPT101B
- [X] 6 Channel Relay
- [X] RTC Module
- [X] Buzzer
- [X] Nextion display

### Tidying Up
#### Pack each variables to global variables
- [X] DS18B20 (Temperature)
- [X] DHT22 (Temperature & Humidity)
- [X] ZMCT101C (AC Current)
- [X] Voltage Current
- [X] 6 Channel Relay
- [X] SD Card Module
- [X] RTC Module
#### Serial Communication with ESP32
- [X] Develop serial communication program to ESP32 (Should be common ground) (Pin -> 13, 12 //RX, TX)
- [X] Convert all data to string

### To Be Done
- [X] DHT22 Constraint: bad soldering from manufacture
- [X] SD Card Module
- [ ] Relay switch feedback for Nextion Display (Dual-State display properties)
- [ ] Relay switch feedback for ESP32 (IoT Dashboard web page feedback)

### Coming Soon (Could be defered)
- [ ] GSP Module NEOBLOX
- [ ] Switch to Arduino Pro Mega for production

### IoT Setup
Notes: ***This controller is to be connected with another slave controller, that is ESP 32 using serial communication. Visit my another repositories to look at the code for ESP32 code.***

# Temporary
printAddress();
buzzerStartFunc();
buzzerSOSFunc();
buzzerInitiating();
loopTemperatureSensors();
loopTemperatureHumidSensor();
temperature1ToLEnthalpy();
temperature2ToEnthalpy();
temperature3ToEnthalpy();
calculateCOP();
calculatePower();
calculateUptime();
calculatePCM1PickLoad();
randomizeFileName();
writeHeaderSDCard();
demoRandomSensingVal();
writeMonitorSDCard();
checkModuleStatus();
loopACCurrent1();
loopTime();
nextionWrite();
updateNextionDisplay();
thingsTogether();
calculateFromThings();