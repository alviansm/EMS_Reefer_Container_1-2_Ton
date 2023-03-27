#include <Arduino.h>
#include <OneWire.h> // DS18B20 Dependency
#include <DallasTemperature.h> // DS18B20 Dependency
#include "DHT.h"
#include <SPI.h> // SD Card Module Dependency
#include <SD.h> // SD Card Module Dependency
#include "uRTCLib.h" // RTC DS1307 Dependency
#include <SoftwareSerial.h> // Serial communication to Nextion/ESP32 dependency

// ======= INITIAL VARIABLES DECLARATIONS =======
// ==== TEMPERATURE SENSORS (DS18B20) ====
#define ONE_WIRE_BUS 4 // Data wire is plugged into port 4 on the Arduino
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

// ==== DHT22 CONFIGURATION ====
#define DHTPIN 8
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

// === BUZZER CONFIGURATION ===
int buzzerPin = 5;

// ==== MICROSD CONFIGURATION ====
File myFile;
const int chipSelect = 10; // change this to match your SD shield or module;

// ==== ZMCT103C CONFIGURATION ====
//#define calibration_const 355.55
#define calibration_const 105.55
// 1st sensor
int accurrent_max_val;
int accurrent_new_val;
int accurrent_old_val = 0;
float accurrent_rms;
float accurrent_IRMS;
// 2nd sensor
int accurrent_max_val2;
int accurrent_new_val2;
int accurrent_old_val2 = 0;
float accurrent_rms2;
float accurrent_IRMS2;
// 3rd sensor
int accurrent_max_val3;
int accurrent_new_val3;
int accurrent_old_val3 = 0;
float accurrent_rms3;
float accurrent_IRMS3;

// ==== ZMPT101B CONFIGURATION ====
double acvoltage_sensorValue1 = 0;
double acvoltage_sensorValue2 = 0;
int acvoltage_crosscount = 0;
int acvoltage_climb_flag = 0;
int acvoltage_val[100];
int acvoltage_max_v = 0;
double acvoltage_VmaxD = 0;
double acvoltage_VeffD = 0;
double acvoltage_Veff = 0;

// ==== RTC DS1307 CONFIGURATION ====
uRTCLib rtc(0x68);
// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // EN
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"}; // ID

// ==== RELAY CONFIGURATION ====
int relay_1 = 34;
int relay_2 = 35;
int relay_3 = 36;
int relay_4 = 37;
int relay_5 = 38;
int relay_6 = 39;
volatile byte relayState = LOW;


// ==== SERIAL COMMUNICATION CONFIGURATION ====
SoftwareSerial arduino(13, 12); // RX, TX
SoftwareSerial lcd(2, 3); //RX, TX

// ======== VARIABLES TO TRACK CONNECTED DEVIES ========
int temperatureSensor = 0;
int temperatureHumidSensor = 0;
int acCurrentSensor = 0;
int acVoltageSensor = 0;
int sdCardModule = 0;
int rtcModule = 0;
int espBoard = 0;
int buzzer = 0;
int reeferRelay = 0;

// ==== MAIN VALUES VARIABLE HOLDER ====
// RTC CLOCK
String rtc_day = "";
String rtc_date = "";
String rtc_clock = "";
// DS18B20 (TEMPERATURE)
// DHT22
// AC VOLTAGE
// AC CURRENT
// RELAY
int relaystate1 = 0;
int relaystate2 = 0;
int relaystate3 = 0;
int relaystate4 = 0;

// ==== VARIABLES TO BE CONTAINED TO AN ARRAY
int monitoredVal [] = {}; // {temperature-1, temperature-2, temperature-3, temperature-4, temperature-5, temperature-6, temperature-7, temperature-humid-1, ac-current-1, ac-current-2, ac-current-3, ac-voltage-1}
int controlledVal [] = {}; // {relay-1, relay-2, relay-3}
int componentStatis [] = {};

// ==== VARIABLES FOR NEXTION HMI DISPLAY ====
// dashboard
String rtcDayVal = "";
String rtcClockVal = "";
String tempInsideVal = "";
String humidInsideVal = "";
String tempAmbientVal = "";
String copVal = "";
String powerVal = "";
String tempPCM1Val = "";
String tempPCM2Val = "";
String assetStatusVal = "";
String uptimeVal = "";
// control

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
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(200);
}
void buzzerSOSFunc(){
  // S
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(200);
  // O
  delay(50);
  tone(buzzerPin, 1000);
  delay(150);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(150);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(150);
  noTone(buzzerPin);
  delay(200);
  // S
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(200);
  delay(50);
}
void buzzerInitiating(){
  //I
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);
  delay(50);
}

// function to loop DS18B20 Temperature Reading
void loopTemperatureSensors() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++) {
    if (numberOfDevices>0) {
      buzzerSOSFunc();
    }
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){

    // Print the data
    float tempC = sensors.getTempC(tempDeviceAddress);
    Serial.print(tempC);
    Serial.print(",");
    }   
  }
  // delay(1000);
  Serial.println();
}

// function to loop trhoush DHT22 Sensor
void loopTemperatureHumidSensor() {
  delay(1000);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
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
void loopACCurrent1() {
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
    
    // delay(1000);
  }
}
void loopACCurrent2() {
  accurrent_new_val2 = analogRead(A8);
  if(accurrent_new_val2 > accurrent_old_val2) {
    accurrent_old_val2 = accurrent_new_val2;
  }  
  else {
    delayMicroseconds(50);
    accurrent_new_val2 = analogRead(A8);
    if(accurrent_new_val2 < accurrent_old_val2) {
      accurrent_max_val2 = accurrent_old_val2;
      accurrent_old_val2 = 0;
    }
    
    accurrent_rms2 = accurrent_max_val2 * 5.00 * 0.707 / 1024;
    accurrent_IRMS2 = accurrent_rms2 * calibration_const;
    
    Serial.print("IRMS2: ");
    Serial.println(accurrent_IRMS2);
    
    // delay(1000);
  }
}
void loopACCurrent3() {
  accurrent_new_val3 = analogRead(A9);
  if(accurrent_new_val3 > accurrent_old_val3) {
    accurrent_old_val3 = accurrent_new_val3;
  }  
  else {
    delayMicroseconds(50);
    accurrent_new_val3 = analogRead(A9);
    if(accurrent_new_val3 < accurrent_old_val3) {
      accurrent_max_val3 = accurrent_old_val3;
      accurrent_old_val3 = 0;
    }
    
    accurrent_rms3 = accurrent_max_val3 * 5.00 * 0.707 / 1024;
    accurrent_IRMS3 = accurrent_rms3 * calibration_const;
    
    Serial.print("IRMS3: ");
    Serial.println(accurrent_IRMS3);
    
    // delay(1000);
  }
}

// function to loop trough ZMPT101B voltage reading
void loopACVoltage() {
    for ( int i = 0; i < 100; i++ ) {
    acvoltage_sensorValue1 = analogRead(A6);
    if (analogRead(A6) > 511) {
      acvoltage_val[i] = acvoltage_sensorValue1;
    }
    else {
      acvoltage_val[i] = 0;
    }
    delay(1);
  }

  acvoltage_max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( acvoltage_val[i] > acvoltage_max_v )
    {
      acvoltage_max_v = acvoltage_val[i];
    }
    acvoltage_val[i] = 0;
  }
  if (acvoltage_max_v != 0) {
    acvoltage_VmaxD = acvoltage_max_v;
    acvoltage_VeffD = acvoltage_VmaxD / sqrt(2);
    acvoltage_Veff = (((acvoltage_VeffD - 420.76) / -90.24) * -210.2) + 110.2;
  }
  else {
    acvoltage_Veff = 0;
  }
  Serial.print("Voltage: ");
  Serial.println(acvoltage_Veff);
  acvoltage_VmaxD = 0;

  // delay(1000);
}

// function to loop trough time using RTC DS1307 Module
void loopTime() {
  rtc.refresh();

  rtc_clock = (rtc.hour()-2);
  rtc_clock.concat(":");
  rtc_clock.concat((rtc.minute())+29);
  rtc_clock.concat(":");
  rtc_clock.concat((rtc.second()));

  rtc_day = daysOfTheWeek[rtc.dayOfWeek()-1];
  rtc_date = rtc.day()+11;
  rtc_date.concat("-");
  rtc_date.concat(rtc.month()+2);
  rtc_date.concat("-");
  rtc_date.concat(rtc.year()+1);
}

// funcion to calculate COP
// function to calculate Power (Watt)
// function to turn off relay 1
void offRelay1(){
  digitalWrite(relay_1, LOW);
}
void offRelay2(){
  digitalWrite(relay_2, LOW);
}
void offRelay3(){
  digitalWrite(relay_3, LOW);
}
void offRelay4(){
  digitalWrite(relay_4, LOW);
}
void offAllRelay() {
  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, LOW);
  digitalWrite(relay_3, LOW);
  digitalWrite(relay_4, LOW);
  digitalWrite(relay_5, LOW);
  digitalWrite(relay_6, LOW);
}
void onRelay1(){
  digitalWrite(relay_1, HIGH);
}
void onRelay2(){
  digitalWrite(relay_2, HIGH);
}
void onRelay3(){
  digitalWrite(relay_3, HIGH);
}
void onRelay4(){
  digitalWrite(relay_4, HIGH);
}

// function to write arduino scripts to nextion display
void nextionWrite() {
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);
}
// function to update HMI value display
void updateNextionDisplay() {
  // Putting Things together
  rtcDayVal = rtc_day;
  rtcDayVal.concat(", ");
  rtcDayVal.concat(rtc_date);
  rtcClockVal = rtc_clock;
  // Dashboard
  lcd.print("rtcDayVal.txt=");
  lcd.print('"');
  lcd.print(rtcDayVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("rtcClockVal.txt=");
  lcd.print('"');
  lcd.print(rtcClockVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("humidInsideVal.txt=");
  lcd.print('"');
  lcd.print(humidInsideVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("tempAmbientVal.txt=");
  lcd.print('"');
  lcd.print(tempAmbientVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("copVal.txt=");
  lcd.print('"');
  lcd.print(copVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("tempPCM1Val.txt=");
  lcd.print('"');
  lcd.print(tempPCM1Val);
  lcd.print('"');
  nextionWrite();;
  lcd.print("tempPCM2Val.txt=");
  lcd.print('"');
  lcd.print(tempPCM2Val);
  lcd.print('"');
  nextionWrite();
  lcd.print("assetStatusVal.txt=");
  lcd.print(assetStatusVal);
  nextionWrite();
  lcd.print("uptimeVal.txt=");
  lcd.print(uptimeVal);
  nextionWrite(); 
  // Details
}

// putting things together
void thingsTogether() {
  loopTime();
}

void setup() {  
    // ==== SETUP FOR ZMCT103C ====
  pinMode(A7, INPUT); // Analog pin for current sensor ZMCT103C, adjust it for the arduino mega
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);

  Serial.begin(9600);
  arduino.begin(9600); // For serial communication
  // ==== SETUP FOR NEXTION DISPLAY ====
  lcd.begin(9600);
  delay(1000);

  // ==== SETUP FOR BUZZER ====
  pinMode(buzzerPin, OUTPUT);

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
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    }
    // } else {
    //   Serial.print("Found ghost device at ");
    //   Serial.print(i, DEC);
    //   Serial.print(" but could not detect address. Check power and cabling");
    // }
  }

  // ==== SETUP FOR DHT22 (TEMPERATURE & HUMIDITY) ====
  dht.begin();

  // ==== SETUP FOR MICROSD ====
  // None

  // ==== SETUP FOR RTC DS1307 ====
  URTCLIB_WIRE.begin();
  // Comment out below line once you set the date & time.
  // Following line sets the RTC with an explicit date & time
  // for example to set January 13 2022 at 12:56 you would call:
  // rtc.set(0, 56, 12, 5, 13, 1, 22);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // set day of week (1=Sunday, 7=Saturday)

  // ==== SETUP FOR 6-CHANNEL RELAY MODULE ====
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);
  pinMode(relay_4, OUTPUT);
  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);
  digitalWrite(relay_3, HIGH);
  digitalWrite(relay_4, HIGH);
  
  // == SETUP READY TRIGGER ===
  // buzzerSOSFunc();
  buzzerInitiating();
}

void loop() {  
  thingsTogether();
  updateNextionDisplay();
  delay(1000);
}