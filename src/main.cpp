#include <Arduino.h>
#include <OneWire.h> // DS18B20 Dependency
#include <DallasTemperature.h> // DS18B20 Dependency
#include <dht.h> // DHT22 Dependency
#include <SPI.h> // SD Card Module Dependency
#include <SD.h> // SD Card Module Dependency

// ======= INITIAL VARIABLES DECLARATIONS =======
// ==== TEMPERATURE SENSORS (DS18B20) ====
#define ONE_WIRE_BUS 4 // Data wire is plugged into port 4 on the Arduino
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

// ==== DHT22 CONFIGURATION ====
#define dataPin 8
dht DHT;

// === BUZZER CONFIGURATION ===
int buzzerPin = 5;

// ==== MICROSD CONFIGURATION ====
File myFile;
const int chipSelect = 10; // change this to match your SD shield or module;

// ==== ZMCT103C CONFIGURATION ====
#define calibration_const 355.55
int accurrent_max_val;
int accurrent_new_val;
int accurrent_old_val = 0;
float accurrent_rms;
float accurrent_IRMS;


// ==== VARIABLES TO TRACK CONNECTED DEVIES ====
int temperatureSensor = 0;
int temperatureHumidSensor = 0;
int acCurrentSensor = 0;
int acVoltageSensor = 0;
int sdCardModule = 0;
int rtcModule = 0;
int espBoard = 0;
int buzzer = 0;
int reeferRelay = 0;

// ==== VARIABLES TO BE CONTAINED TO AN ARRAY
int monitoredVal [] = {}; // {temperature-1, temperature-2, temperature-3, temperature-4, temperature-5, temperature-6, temperature-7, temperature-humid-1, ac-current-1, ac-current-2, ac-current-3, ac-voltage-1}
int controlledVal [] = {}; // {relay-1, relay-2, relay-3}
int componentStatis [] = {};

// ======== INITIAL FUNCTIONS DECLARATIONS ========
// ==== TEMPERATURE SENSORS (DS18B20) ====
// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

// function to start buzzer
void buzzerStartFunc() {
  tone(buzzerPin, 1000);
  delay(100);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, 1000);
  delay(100);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, 1000);
  delay(100);
  noTone(buzzerPin);
}

// function to loop DS18B20 Temperature Reading
void loopTemperatureSensors() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){

    // Print the data
    float tempC = sensors.getTempC(tempDeviceAddress);
    Serial.print(tempC);
    Serial.print(",");
    }   
  }
  delay(1000);
  Serial.println();
}

// function to loop trhoush DHT22 Sensor
void loopTemperatureHumidSensor() {
  int readData = DHT.read22(dataPin); // DHT22/AM2302
  float t = DHT.temperature;
  float h = DHT.humidity;
  Serial.print(t);
  Serial.print(h);
  delay(1000);
}

// function to write values in monitor array
void writeMonitorSDCard() {
  myFile = SD.open("eco_reefer_container_data.csv", FILE_WRITE);
  if (myFile) {
    // if the data could be opened
    int len = sizeof(monitoredVal)/sizeof(monitoredVal[0]);
    for(int i =0; i<len; i++) {
      myFile.print(monitoredVal[i]);
      if (!(i == len-1)) {
        myFile.print(",");
      }
    }
    myFile.println(",");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening eco_reefer_container_data.csv");
  }
}

// function to loop SINGLE AC current sensor ZMCT103C
void loopACCurrent() {
  accurrent_new_val = analogRead(A7);
  if(accurrent_new_val > accurrent_old_val) {
    accurrent_old_val = accurrent_new_val;
  }
  
  else {
    delayMicroseconds(50);
    accurrent_new_val = analogRead(A7);
    if(accurrent_new_val < accurrent_old_val) {
      accurrent_max_val = accurrent_old_val;
      accurrent_old_val = 0;
    }
    
    accurrent_rms = accurrent_max_val * 5.00 * 0.707 / 1024;
    accurrent_IRMS = accurrent_rms * calibration_const;
    
    Serial.print("IRMS: ");
    Serial.println(accurrent_IRMS);
    
    delay(1000);
  }
}

void setup() {  
  Serial.begin(9600);

  // ==== SETUP FOR BUZZER ====
  pinMode(buzzerPin, OUTPUT);
  buzzerStartFunc();

  // ==== SETUP FOR TEMPERATURE SENSORS ====
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      if (i > 0){
        buzzerStartFunc();
      }
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }

  // ==== SETUP FOR DHT22 (TEMPERATURE & HUMIDITY) ====
  // None

  // ==== SETUP FOR MICROSD
  // None

  // ==== SETUP FOR ZMCT103C
  pinMode(A7, INPUT); // Analog pin for current sensor ZMCT103C, adjust it for the arduino mega

}

void loop() {
  loopTemperatureSensors();
  loopTemperatureHumidSensor();
}