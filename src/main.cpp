/*
  Written by: Alvians Maulana
  Project Name: Development of IoT-Based Energy Management System of Hybrid Refrigeration Cycle (Phase Change Material - Vapour Compression Cycle) for Eco-Reefer Container
  Year: 2023
*/
#include <Arduino.h>
#include <OneWire.h> // DS18B20 Dependency
#include <DallasTemperature.h> // DS18B20 Dependency
#include <Adafruit_Sensor.h>
#include <DHT.h>
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
#define DHTTYPE DHT22
DHT dht = DHT(DHTPIN, DHTTYPE);

// === BUZZER CONFIGURATION ===
int buzzerPin = 5;

// ==== MICROSD CONFIGURATION ====
File myFile;
const int chipSelect = 53; // change this to match your SD shield or module;

// ==== ZMCT103C CONFIGURATION ====
//#define calibration_const 355.55
#define calibration_const 355.55
// 1st sensor
int accurrent_max_val;
int accurrent_new_val;
int accurrent_old_val = 0;
float accurrent_rms;
float accurrent_IRMS;

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

// === LED FOR INDICATOR CONFIGURATION ====
#define LED1 41
#define LED2 42
#define LED3 43
#define LED4 44
#define LED5 45
#define LED6 46
#define LED7 47
#define LED8 48
#define LED9 49

// ======== VARIABLES TO TRACK CONNECTED DEVIES ========
int statusTemperatureSensor = 0;
int statusHumiditySensor = 0;
int statusCurrentSensor = 0;
int statusVoltageSensor = 0;
int statusSDCardModule = 0;
int statusRTCModule = 0;
int statusESPBoard = 0;
int statusBuzzerModule = 0;
int statusRelayModule = 0;

// ==== MAIN VALUES VARIABLE HOLDER ====
// RTC CLOCK
String rtc_day = "";
String rtc_date = "";
String rtc_clock = "";
String rtc_clock_minute = "";
// RELAY
int relaystate1 = 0;
int relaystate2 = 0;
int relaystate3 = 0;
int relaystate4 = 0;

// ==== SENSING VARIABLES FROM SENSOR ====
// temporary used, to be changed to an array later. (if there's available time)
// RTC DS1307 Modules
String senseTime = "";
// DS18B20
String senseTemp1 = "";
String senseTemp2 = "";
String senseTemp3 = "";
String senseTemp4 = "";
String senseTemp5 = "";
String senseTemp6 = "";
String senseTemp7 = "";
int sensorIteration = 0;
// DHT22
String senseTemp8 = "";
String senseHumid = "";
// ZMCT101C
String senseCurrent1 = "";
String senseCurrent2 = "";
String senseCurrent3 = "";
// ZMPT101
String senseVoltage1 = "";
// Raw Pressure Transducer 1
String sensePressureTransducer1 = "";
// Raw Pressure Transducer 2
String sensePressureTransducer2 = "";
// Raw Pressure Transducer 3
String sensePressureTransducer3 = "";

// ==== VARIABLES TO CALCUALTE COPs ====
String calculatedCOP = "";
float enthalpy4 = 0.00;
float enthalpy3 = 0.00;
float enthalpy2 = 0.00;
float enthalpy1 = 0.00;
float tempOutEvap = 0.00;
float tempInEvap = 0.00;

String calculatedPower = "";
String calculatedUptime = "";
String calculatedPCM1Pickload = "";

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
// details
String detailsT1 = "";
String detailsT2 = "";
String detailsT3 = "";
String detailsT4 = "";
String detailsT5 = "";
String detailsT6 = "";
String detailsT7 = "";
String detailsT8 = "";
String detailsVoltage = "";
String detailsRH = "";
String detailsPower = "";
String detailsCOP = "";
String detailsI1 = "";
String detailsPick = "";
String detailsFP = "";
String detailsUptime = "";
String detailsIter = "";
String detailsPrice = "";

// ==== VARIABLES FOR MICROSD - LOCAL STORAGE ====
String SDCardFileName = "";
String completeRTC1SD = "";
String temp1SD = "";
String temp2SD = "";
String temp3SD = "";
String temp4SD = "";
String temp5SD = "";
String temp6SD = "";
String temp7SD = "";
String temp8SD = "";
String humidSD = "";
String current1SD = "";
String power1SD = "";
String cop1SD = "";
String pcm1PickloadSD = "";
String pcm1FrozenPointSD = "";
String uptime1SD = "";
String iteration1SD = "";
String price1SD = "";

String globalCompleteSDCardData = "";

// ======== INITIAL FUNCTIONS DECLARATIONS ========
void thingsTogether();
void calculateFromThings();
void updateNextionDisplay();
void printAddress(DeviceAddress deviceAddress);
void buzzerStartFunc();
void buzzerSOSFunc();
void buzzerInitiating();
void loopTemperatureSensors();
void loopTemperatureHumidSensor();
void temperature1ToLEnthalpy();
void temperature2ToEnthalpy();
void temperature3ToEnthalpy();
void calculateCOP();
void calculatePower();
void calculateUptime();
void calculatePCM1PickLoad();
void randomizeFileName();
void writeHeaderSDCard();
void demoRandomSensingVal();
void writeMonitorSDCard();
void checkModuleStatus();
void loopACCurrent1();
void loopTime();
void nextionWrite();

// ======== MAIN PROGRAM TO BE EXECUTED ========
void setup() {  
  // ==== SETUP FOR ZMCT103C ====
  pinMode(A7, INPUT); // Analog pin for current sensor ZMCT103C, adjust it for the arduino mega
  // pinMode(A8, INPUT);
  // pinMode(A9, INPUT);

  Serial.begin(9600);
  arduino.begin(9600); // For serial communication
  // ==== SETUP FOR NEXTION DISPLAY ====
  lcd.begin(9600);
  delay(500);

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
  Serial.print("Initializing SD Card...");
  if(!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    buzzerSOSFunc();
    statusSDCardModule = 0;
  }
  Serial.println("SD card initialization done.");
  statusSDCardModule = 1;
  writeHeaderSDCard();

  // ==== SETUP FOR RTC DS1307 ====
  URTCLIB_WIRE.begin();

  // ==== SETUP FOR 6-CHANNEL RELAY MODULE (4 only used) ====
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);
  pinMode(relay_4, OUTPUT);
  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);
  digitalWrite(relay_3, HIGH);
  digitalWrite(relay_4, HIGH);

  // ==== SETUP FOR LED INDICATOR ====
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  pinMode(LED9, OUTPUT);
  
  // == SETUP READY TRIGGER ===
  buzzerInitiating();
}

void loop() {  
  // demoRandomSensingVal();
  checkModuleStatus();
  thingsTogether();
  calculateFromThings();
  writeMonitorSDCard();
  updateNextionDisplay();
  delay(1000);
}

// ======= FUNCTION SETUP =======
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

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
    if (numberOfDevices<1) {
      buzzerSOSFunc();
    }
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){

    // Print the data
    float tempC = sensors.getTempC(tempDeviceAddress);
    Serial.print(tempC);
    Serial.print(",");
    
    // insert sensor
    if (i==0) {
      senseTemp1 = tempC;
    }
    if (i==1) {
      senseTemp2 = tempC;
    }
    if (i==2) {
      senseTemp3 = tempC;
    }
    if (i==3) {
      senseTemp4 = tempC;
    }
    if (i==4) {
      senseTemp6 = tempC;
    }
    if (i==5) {
      senseTemp6 = tempC;
    }
    if (i==6) {
      senseTemp7 = tempC;
    }

    }   
  }
  // delay(1000);
  Serial.println();
}

// function to loop trhoush DHT22 Sensor
void loopTemperatureHumidSensor() {
  delay(2000);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.print("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  // insert sensor reading to global variable
  senseTemp8 = t;
  senseHumid = h;
}

void temperature1ToLEnthalpy(){
  // Enthalpy at state 1 is determined by linear regression using equation that obtained by using Freon R-404a saturation table.
  // State 1 of the cycle is supposed to be sat. vapor
  float temperatureState1 = 0.00;
  enthalpy1 = ((0.5334*(temperatureState1))+363.52); 
}

void temperature2ToEnthalpy() {
  // State 2 of the cycle is supposed to be sat. vapor
  float temperatureState2 = 0.00;
  enthalpy2 = ((0.5334*temperatureState2)+363.52);
}

void temperature3ToEnthalpy() {
  // State 3 of the cycle is supposed to be sat. liquid
  float temperatureState3 = 0.00;
  enthalpy3 = ((1.4143*temperatureState3)+206.36);
  // Assuming ideal cycle, enthalpy at state 4 is equal to enthalpy at state 3
  enthalpy4 = enthalpy3;
}

void calculateCOP() {
  calculatedCOP = String((enthalpy1-enthalpy4)/(enthalpy2-enthalpy1));
}

void calculatePower() {
  calculatedPower = String(((senseCurrent1.toFloat())*220.00*0.80));
}

void calculateUptime() {
  String previous_minute = "";

  previous_minute = rtc_clock_minute;
  rtc_clock_minute = rtc.minute();
  if (rtc_clock_minute != previous_minute) {
    calculatedUptime = calculatedUptime+1;
  }  
}

void calculatePCM1PickLoad() {
  calculatedPCM1Pickload = "6"; // 6 hours by default
}

void randomizeFileName() {
  SDCardFileName = "";
  int random_length = 4;
  int temporary_random_num = random(0, 9);
  SDCardFileName = String(temporary_random_num);

  for (int i=0;i<random_length;i++){
    temporary_random_num = random(0,9);
    SDCardFileName.concat(String(temporary_random_num));
  }

  // adding format file
  SDCardFileName.concat(".CSV");
}

void writeHeaderSDCard() {
  SDCardFileName = "";
  randomizeFileName();

  delay(500);
  myFile = SD.open(SDCardFileName, FILE_WRITE);
  if (myFile) {
    // if the data could be opened
    // print heading to sd card
    statusSDCardModule = 1;
    // myFile.println("time,temperature_1,temperature_2,temperature_3,temperature_4,temperature_5,temperature_6,temperature_7,current_1,rh_1,power_1,cop_1,pcm_pickload,pcm_forzen_point,uptime,iteration,electric_bill_per_kwh,raw_signal_pressure_1,raw_signal_pressure_2,raw_signal_pressure_3");
    myFile.println("time,temperature_1,temperature_2,temperature_3,temperature_4,temperature_5,temperature_6,temperature_7,temperature_8,current_1,rh_1,power_1,cop_1,pcm_pickload,pcm_forzen_point,uptime,iteration,electric_bill_per_kwh");       
    // close the sd card
    myFile.close();
  } 
  else {    
    statusSDCardModule = 0;
    Serial.print("Error opening ");
    Serial.print(SDCardFileName);
    Serial.println();
    buzzerSOSFunc();
  }
}

void demoRandomSensingVal() {
  senseTemp1 = String(random(-30, 30));
  senseTemp2 = String(random(-30, 30));
  senseTemp3 = String(random(-30, 30));
  senseTemp4 = String(random(-30, 30));
  senseTemp5 = String(random(-30, 30));
  senseTemp6 = String(random(-30, 30));
  senseTemp7 = String(random(-30, 30));
  senseHumid = String(random(0, 100));
  senseCurrent1 = String(random(-30, 30));
  senseCurrent2 = String(random(-30, 30));
  senseCurrent3 = String(random(-30, 30));
  senseVoltage1 = String(random(-30, 30));
  
  temp1SD = senseTemp1;
  temp2SD = senseTemp2;
  temp3SD = senseTemp3;
  temp4SD = senseTemp4;
  temp5SD = senseTemp5;
  temp6SD = senseTemp6;
  temp7SD = senseTemp7;
  humidSD = senseHumid;
  current1SD = senseCurrent1;

  tempInsideVal = senseTemp1;
  tempAmbientVal = senseTemp2;
  tempPCM1Val = senseTemp3;
  tempPCM2Val = senseTemp4;
  humidInsideVal = senseHumid;
}

void writeMonitorSDCard() {
  // please run this function after time loop ds1307 module so the date could be written properly
  completeRTC1SD = senseTime;
  humidSD = senseHumid;
  temp1SD = senseTemp1;
  temp2SD = senseTemp2;
  temp3SD = senseTemp3;
  temp4SD = senseTemp4;
  temp5SD = senseTemp5;
  temp6SD = senseTemp6;
  temp7SD = senseTemp7;
  temp8SD = senseTemp8;
  current1SD = senseCurrent1;

  // Assumption & calculation
  uptime1SD = calculatedUptime;
  power1SD = calculatedPower;
  pcm1PickloadSD = calculatedPCM1Pickload;
  pcm1FrozenPointSD = "-22";
  price1SD = "1699.53";
  iteration1SD = "1";

  myFile = SD.open(SDCardFileName, FILE_WRITE);
  String completeDataPerRowSD = "";
  if (myFile) {
    // check if any of the data has value
    if ((completeRTC1SD.length()>0) || (temp1SD.length()>0) || (temp2SD.length()>0) || (temp2SD.length()>0) || (temp3SD.length()>0) || (temp4SD.length()>0) || (temp5SD.length()>0) || (temp6SD.length()>0) || (temp7SD.length()>0) || (temp8SD.length()>0) || (current1SD.length()>0) || (humidSD.length()>0) || (power1SD.length()>0) || (cop1SD.length()>0) || (pcm1PickloadSD.length()>0) || (pcm1FrozenPointSD.length()>0) || (uptime1SD.length()>0) || (iteration1SD.length()>0) || (price1SD.length()>0)) {
    // concat all available data for sd card
    completeDataPerRowSD.concat(completeRTC1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp2SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp3SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp4SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp5SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp6SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp7SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp8SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(current1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(humidSD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(power1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(cop1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(pcm1PickloadSD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(pcm1FrozenPointSD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(uptime1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(iteration1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(price1SD);

      // print data to sd card
      myFile.println(completeDataPerRowSD);
      myFile.close();

      globalCompleteSDCardData = completeDataPerRowSD;
    }
  } else {
    // if the file didn't open, print an error:
    Serial.print("error writing ");
    Serial.print(SDCardFileName);
    Serial.println();

    // concat all available data for sd card
    completeDataPerRowSD.concat(completeRTC1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp2SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp3SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp4SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp5SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp6SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp7SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(temp8SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(current1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(humidSD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(power1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(cop1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(pcm1PickloadSD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(pcm1FrozenPointSD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(uptime1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(iteration1SD);
    completeDataPerRowSD.concat(",");
    completeDataPerRowSD.concat(price1SD);
    // completeDataPerRowSD.concat(",");
    // completeDataPerRowSD.concat(sensePressureTransducer1);
    // completeDataPerRowSD.concat(",");
    // completeDataPerRowSD.concat(sensePressureTransducer2);
    // completeDataPerRowSD.concat(",");
    // completeDataPerRowSD.concat(sensePressureTransducer3);

    globalCompleteSDCardData = completeDataPerRowSD;
  }
}

void checkModuleStatus() {
  if (senseTemp1.length() > 0 && senseTemp2.length() > 0 && senseTemp3.length() > 0 && senseTemp4.length() > 0 && senseTemp5.length() > 0 && senseTemp6.length() > 0 && senseTemp7.length() > 0) {
    statusTemperatureSensor = 1;
  } else {
    statusTemperatureSensor = 0;
  }

  if (senseHumid.length() > 0) {
    statusHumiditySensor = 1;
  } else {
    statusHumiditySensor = 0;
  }

  if (senseCurrent1.length() > 0 && senseCurrent2.length() > 0 && senseCurrent3.length() > 0) {
    statusCurrentSensor = 1;
  } else {
    statusCurrentSensor = 0;
  }

  if (senseVoltage1.length() > 0) {
    statusVoltageSensor = 1;
  } else {
    statusVoltageSensor = 0;
  }

  if (senseTime.length() > 0) {
    statusRTCModule = 1;
  } else {
    statusRTCModule = 0;
  } 

  //
  if (statusTemperatureSensor == 1) {
    digitalWrite(LED1, HIGH);
  } else {
    digitalWrite(LED1, LOW);
  }
  if (statusHumiditySensor == 1) {
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(LED2, LOW);
  }
  if (statusCurrentSensor == 1) {
    digitalWrite(LED3, HIGH);
  } else {
    digitalWrite(LED3, LOW);
  }
  if (statusVoltageSensor == 1) {
    digitalWrite(LED4, HIGH);
  } else {
    digitalWrite(LED4, LOW);
  }
  if (statusSDCardModule == 1) {
    digitalWrite(LED5, HIGH);
  } else {
    digitalWrite(LED5, LOW);
  }
  if (statusRTCModule == 1) {
    digitalWrite(LED6, HIGH);
  } else {
    digitalWrite(LED6, LOW);
  }
  if (statusESPBoard == 1) {
    digitalWrite(LED7, HIGH);
  } else {
    digitalWrite(LED7, LOW);
  }
  if (statusBuzzerModule == 1) {
    digitalWrite(LED8, HIGH);
  } else {
    digitalWrite(LED8, LOW);
  }
  if (statusRelayModule == 1) {
    digitalWrite(LED9, HIGH);
  } else {
    digitalWrite(LED9, LOW);
  }

}

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
    
    // insert sensor reading to global value
    senseCurrent1 = accurrent_IRMS;

    // delay(1000);
  }
}

void loopACCurrent1_alternative() {
  return;
}

void loopTime() {
  rtc.refresh();

  rtc_clock = (rtc.hour());
  rtc_clock.concat(":");
  rtc_clock.concat((rtc.minute()));
  rtc_clock.concat(":");
  rtc_clock.concat((rtc.second()));

  rtc_day = daysOfTheWeek[rtc.dayOfWeek()];
  rtc_date = rtc.day();
  rtc_date.concat("-");
  rtc_date.concat(rtc.month());
  rtc_date.concat("-");
  rtc_date.concat(rtc.year());

  senseTime = rtc_day;
  senseTime.concat("-");
  senseTime.concat(rtc_date);
  senseTime.concat("-");
  senseTime.concat(rtc_clock);
}

void nextionWrite() {
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);
}

void displayVariableAcquire() {
  // dashaboard
  tempInsideVal = "";
  humidInsideVal = "";
  tempAmbientVal = "";
  copVal = "";
  tempPCM1Val = "";
  tempPCM2Val = "";
  assetStatusVal = "";
  powerVal = "";
  uptimeVal = "";
  // details
  detailsT1 = senseTemp1;
  detailsT2 = senseTemp2;
  detailsT3 = senseTemp3;
  detailsT4 = senseTemp4;
  detailsT5 = senseTemp5;
  detailsT6 = senseTemp6;
  detailsT7 = senseTemp7;
  detailsT8 = senseTemp8;
  detailsVoltage = senseVoltage1;
  detailsRH = senseHumid;
  detailsPower = calculatedPower;
  detailsCOP = calculatedCOP;
  detailsI1 = senseCurrent1;
  detailsPick = "";
  detailsFP = "";
  detailsUptime = "";
  detailsIter = "";
  detailsPrice = "";
}

void updateNextionDisplay() {
  // variable acquisition to be displayed
  displayVariableAcquire();
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
  lcd.print("tempInsideVal.txt=");
  lcd.print('"');
  lcd.print(tempInsideVal);
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
  lcd.print('"');
  lcd.print(assetStatusVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("powerVal.txt=");
  lcd.print('"');
  lcd.print(powerVal);
  lcd.print('"');
  nextionWrite();
  lcd.print("uptimeVal.txt=");
  lcd.print('"');
  lcd.print(uptimeVal);
  lcd.print('"');
  nextionWrite(); 
  // Details
  lcd.print("detailsT1.txt=");
  lcd.print('"');
  lcd.print(detailsT1);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT2.txt=");
  lcd.print('"');
  lcd.print(detailsT2);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT3.txt=");
  lcd.print('"');
  lcd.print(detailsT3);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT4.txt=");
  lcd.print('"');
  lcd.print(detailsT4);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT5.txt=");
  lcd.print('"');
  lcd.print(detailsT5);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT6.txt=");
  lcd.print('"');
  lcd.print(detailsT6);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT7.txt=");
  lcd.print('"');
  lcd.print(detailsT7);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsT8.txt=");
  lcd.print('"');
  lcd.print(detailsT8);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsVoltage.txt=");
  lcd.print('"');
  lcd.print(detailsVoltage);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsRH.txt=");
  lcd.print('"');
  lcd.print(detailsRH);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsPower.txt=");
  lcd.print('"');
  lcd.print(detailsPower);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsCOP.txt=");
  lcd.print('"');
  lcd.print(detailsCOP);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsI1.txt=");
  lcd.print('"');
  lcd.print(detailsI1);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsPick.txt=");
  lcd.print('"');
  lcd.print(detailsPick);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsFP.txt=");
  lcd.print('"');
  lcd.print(detailsFP);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsUptime.txt=");
  lcd.print('"');
  lcd.print(detailsUptime);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsIter.txt=");
  lcd.print('"');
  lcd.print(detailsIter);
  lcd.print('"');
  nextionWrite();
  lcd.print("detailsPrice.txt=");
  lcd.print('"');
  lcd.print(detailsPrice);
  lcd.print('"');
  nextionWrite();
}

void thingsTogether() {
  loopTime();
  loopTemperatureHumidSensor();
  loopTemperatureSensors();
  loopACCurrent1();
}

void calculateFromThings() {
  calculateCOP();
  calculatePower();
  calculateUptime();
  calculatePCM1PickLoad();
}