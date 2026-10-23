
/*SHC - The One Plane: Full Software Code; Collective of All Software For The RCA Team #1 Aircraft
  Created: 7 October 2026 | Modified: 22 October 2026 | By Arwyn & Seito*/
#include <Wire.h> //Core I2C Communication
#include <Adafruit_Sensor.h>  //General Altitude & Pressure Sensing
#include "Adafruit_BMP3XX.h"  //BMP388 - Altitude & Pressure Sensing
#include <Adafruit_BNO055.h>  //BNO055 - Motion & Orientation Sensing
#include <utility/imumaths.h> //Motion & Orientation Sensing
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //GPS - Location Tracking | http://librarymanager/All#SparkFun_u-blox_GNSS
#include <SHC_M9N.h>  //GPS - Speed Tracking | Provided By Sam (We Love You Sam)
#include <Utils.h>  //Basic Utilities
#include <SPI.h>  //Enables SPI Communication For Data Logging
#include <SD.h> //Data Logging For SD Card

//Pins & Sensor Set-Up (HC12 Radio Does Not Need Configured Pins, But Pins Are: txPin 0 rxPin 1 HC12 Serial1):
  #define SDA 4 //I2C Manual Configuration
  #define SCL 5 //I2C Manual Configuration
  #define DOOR_PIN 17 //Detects A Signal From Controller When Door Opens
  #define chipSelect 20 //Only Neccessary Pin For SD Card | Other Pins: SD_SO 16 SD_CLK 18 SD_SI 19*/
  #define BUZZER_PIN 21 //Connect Buzzer
  #define LED_WHITE 22  //Connect Blinking LED
  SFE_UBLOX_GNSS myGNSS;  //Set-Up GNSS
  M9N gps = M9N(); //Set-Up GPS  WithThe Help Of Premade SHC Library (Love You Sam! :3)
  Adafruit_BMP3XX bmp;  //Set-Up BMP3XX
  Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); //Check & Set I2C Device Address

//Global - Flight Conditions
  bool onLED = false; //LED Starts As LOW (Off)
  bool doorOpen = false;  //Did The Door Ever Open (Can The Buzzer Play?)
  bool songTime = false; //Did The Sand Deployment Doors Open? (Did The PICO Recieve Signal From Door Pin?)
//Global - Sensor Data (-999999 As Error-Handling Value):
  double storeTime = -999999; //Time Sensors Took Data (For SD Card)
  double temperature = -999999;
  double pressure = -999999;
  double altitude = -999999;
  double decimalLat = -999999;
  double decimalLong = -999999;
  double roll = -999999;
  double pitch = -999999;
  double yaw = -999999;
  double accelX = -999999;
  double accelY = -999999;
  double accelZ = -999999;
  double speed = -999999;
  String dataString = ""; //String Used For Data Storage
  String fileName;  //File Name For SD Card
//Global - Conditional Cycles (Rather Than Delays, The Miliseconds Run Between Each Process Type):
  const int msBMPPullRate = 200;
  int msLastBMPPull = 0;
  const int msBNOPullRate = 200;
  int msLastBNOPull = 0;
  const int msGNSSPullRate = 400; //Higher Rate Between GNSS Data Pulls To Limit SPI Traffic
  int msLastGNSSPull = 0;
  const int msWriteRate = 200;
  int msLastWrite = 0;
  const int msRadioRate = 200;
  int msLastRadio = 0;
  const int msBlinkRate = 500;  //Required To Be 500ms
  int msLastBlink = 0;
//Global - Music (Songs: Mr. Sandman & Enter Sandman):
  float noteDuration = 0;
  int msLastNote = 0; //Time Since Last Note Played
  int noteSeq = 0;  //Current Note In Sequence; Returns To 0 For Repeats
  bool mrSandman = true;  //Mr. Sandman Plays First, Then Switches To Enter Sandman; Only Two Songs, Boolean More Optimal
  //Notes & Voltage:
    #define REST     0
    #define NOTE_D3  146.832
    #define NOTE_E3  164.814
    #define NOTE_F3  174.614
    #define NOTE_G3  195.998
    #define NOTE_A3  220.000
    #define NOTE_AS3 233.082
    #define NOTE_B3  246.942
    #define NOTE_C4  261.626
    #define NOTE_D4  293.665
    #define NOTE_DS4 311.127
    #define NOTE_E4  329.628
    #define NOTE_F4  349.228
    #define NOTE_G4  391.995
    #define NOTE_A4  440.000
    #define NOTE_B4  493.883
    #define NOTE_C5  523.251
  //Mr. Sandman:
    const int mrBPM = 142;
    const float mrNotesArr[] {
      NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4,NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_A3, NOTE_E3, NOTE_E3, NOTE_D3, NOTE_E3, NOTE_G3, REST, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_DS4, NOTE_DS4, NOTE_E4, NOTE_D4, REST, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, REST, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, NOTE_D3, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, REST, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_C5, REST
    };
    const float mrDurations[] {
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 4, 8, 2, 8, 8, 8, 4, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 4, 4, 8, 4, 8, 8, 4, 8, 2, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 8, 4, 8, 8, 8, 2, 4, 8, 8, 4, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 1, 8, 0.25
    };
    const int mrSize = sizeof(mrNotesArr) / sizeof(int);  //Amount Of Notes; Automatically Updates When Note List Changes
  //Enter Sandman:
    const int etrBPM = 130;
    const float etrNotesArr[] {
      NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_G3, REST, NOTE_E3, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_G3, REST, NOTE_E3,NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_G3, REST, NOTE_E3, REST
    };
    const float etrDurations[] {
      4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 8, 2, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 8, 2, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 8, 2, 0.25
    };
    const int etrSize = sizeof(etrNotesArr) / sizeof(int); //Amount Of Notes; Automatically Updates When Note List Changes

void initPinModes() //Set-Up Pins
{
  Serial1.begin(9600);  //Sets HC12 Radio; Other Pins: pinMode(rxPin, INPUT); pinMode(txPin, OUTPUT);*/
  pinMode(BUZZER_PIN, OUTPUT);//Determine Pin Type For Buzzer (OUTPUT - Higher Voltage)
  pinMode(LED_WHITE, OUTPUT); //Determine Pin Type For Blinking LED (OUTPUT - Higher Voltage)
  pinMode(DOOR_PIN,INPUT);  //Determine Pin Type For Door Pin (INPUT - Detects Signals)
  Wire.begin(); //I2C Configuration
  Wire.setSDA(SDA);
  Wire.setSCL(SCL);
}

void initBMP() //BMP388 Initialization
{ 
  bmp.begin_I2C();  //Set-Up With I2C
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void initGNSS() //GNSS Initialization
{
  gps.init();
  myGNSS.begin(Wire); //Connect To GNSS Using SPI Port
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set I2C Port To Output UBX Protocol Data Only
  myGNSS.setAutoPVT(true);
}

void newSDFile()  //Creates New .csv File For SD Card Data
{
  int fileNum = 0;
  do {  //Dynamically Create A Name For .csv File
    fileNum++;
    fileName = "datalog" + String(fileNum) + ".csv";
  }
  while(SD.exists(fileName)); //If This File Name Doesn't Exist Yet, Break And Use That File
  File dataFile = SD.open(fileName, FILE_WRITE); //Open File (Only One File Can Open At A Time)
  if(dataFile) { //If File Is Available, Write To It
    dataFile.println("Time,Longitude,Latitude,Altitude,Pressure,Temperature,Roll,Pitch,Yaw,Ax,Ay,Az,Speed");  //Headers At Top Of Table
    dataFile.close();
  }
}

void blinkLED() //Blink The LED
{
  if(!onLED)
    digitalWrite(LED_WHITE, HIGH);
  else
    digitalWrite(LED_WHITE, LOW);
  onLED = !onLED;
}

void playNote() //Play Current Note, And Prepare For The Next
{
  noTone(BUZZER_PIN); //Stop Playing On The Buzzer
  if((mrSandman && noteSeq == mrSize) || (!mrSandman && noteSeq == etrSize)) {  //If All Notes Have Played, Onto The Next Song (Mr. Sandman <-> Enter Sandman)!
    noteSeq = 0;
    mrSandman = !mrSandman;
  }
  if(mrSandman) { //Playing Mr. Sandman
    noteDuration = (60.0 / float(mrBPM) / (float(mrDurations[noteSeq])/4.0) * 1000);
    tone(BUZZER_PIN, mrNotesArr[noteSeq], noteDuration);
  }
  else {
    noteDuration = (60.0 / float(etrBPM) / (float(etrDurations[noteSeq])/4.0) * 1000);
    tone(BUZZER_PIN, etrNotesArr[noteSeq], noteDuration);
  }
  noteSeq++;
}

bool readBMP(int attempts)  //Read Data From BMP388
{
  if(bmp.performReading()) { //Ensure BMP388 Reads Correctly; Sets Error-Handling Values If Not
    temperature = (bmp.temperature * 1.8) + 32;
    pressure = bmp.pressure / 100.0 / 68.948;
    return true;
  }
  temperature = -999999;
  pressure = -999999;
  if(attempts > 0)
    readBMP(attempts - 1);
  return false;
}

bool readBNO()  //Read Data From BNO055
{
  sensors_event_t orientationData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  if(orientationData.type == SENSOR_TYPE_ORIENTATION) {
    yaw = orientationData.orientation.x;       
    roll = orientationData.orientation.y; 
    pitch = orientationData.orientation.z; 
  } 
  if(linearAccelData.type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    accelX = linearAccelData.acceleration.x * 3.28084;
    accelY = linearAccelData.acceleration.y * 3.28084;
    accelZ = linearAccelData.acceleration.z * 3.28084;
  }
  return true;
}

bool readGNSS(int attempts) //Read Data From GNSS
{
  gps.prefetchData();
  byte SIV = gps.getSIV();
  if(gps.getSIV() > 3) {  //If GNSS Can Read Data
    decimalLat = gps.getLatitude();
    decimalLong = gps.getLongitude();
    altitude = gps.getAltitude()* 3.28084;
    speed = myGNSS.getGroundSpeed()/447.04;
    return true;
  }
  decimalLat = -999999;
  decimalLong = -999999;
  altitude = -999999;
  speed = -999999;
  if(attempts > 0)
    readGNSS(attempts - 1);
  return false;
}

void dataCompile()  //Compile All Sensor Data Into A String
{
  dataString = String(decimalLong , 6) + ",";
  dataString += String(decimalLat , 6) + ",";
  dataString += String(altitude , 2) + ",";
  dataString += String(pressure , 2) + ",";
  dataString += String(temperature , 2) + ",";
  dataString += String(roll , 2) + ",";
  dataString += String(pitch , 2) + ",";
  dataString += String(yaw , 2) + ",";
  dataString += String(accelX , 2) + ",";
  dataString += String(accelY , 2) + ",";
  dataString += String(accelZ , 2) + ",";
  dataString += String(speed , 2);
}

bool writeToSD()  //Send Data To SD Card
{
  File dataFile = SD.open(fileName, FILE_WRITE); //Open File (Only One File Can Open At A Time)
  if(dataFile) { //If File Is Available, Write To It
    dataFile.println(String(storeTime) + "," + dataString); //Time Not Native To SD Card, But Is To HC12 Radio; dataString Excludes Time Accordingly
    dataFile.close();
    return true;
  }
  return false;
}

void setup() {  //Run All Set-Up Controls
  initPinModes();
  initBMP();
  bno.begin();
  initGNSS();
  SD.begin(chipSelect); //Checks If SD Card Initialized For Pin
  newSDFile();
}

void loop() { //Loops & Conditional Cycles
//Blink LED:
  if(millis() - msLastBlink >= msBlinkRate) { //500ms Conditional Cycle Requirement; Checks When 500ms Passed Between Run
    blinkLED();
    msLastBlink = millis();
  }
//Buzzer Melody:
  if(pulseIn(DOOR_PIN, HIGH, 50000) < 1400) //Check When Door Opens | Check If Pulse Matches When Controller Switch Flipped
    doorOpen = true;
  if(!songTime && doorOpen) //When Door Opens, It Is Song Time!
    songTime = true;
  if((songTime) && (millis() - msLastNote >= noteDuration * 1.30)) {  //Small Pauses Between Each Note
    playNote();
    msLastNote = millis();
  }
//Output Sensor Data:
  storeTime = millis(); //Time When Sensors Take Data
  if(millis() - msLastBMPPull >= msBMPPullRate) {
    if(readBMP(3))
      msLastBMPPull = millis();
  }
  if(millis() - msLastBNOPull >= msBNOPullRate) {
    if(readBNO())
      msLastBNOPull = millis();
  }
  if(millis() - msLastGNSSPull >= msGNSSPullRate) {
    if(readGNSS(2))
      msLastGNSSPull = millis();
  }
//Send Data To SD Card & HC12 Radio:
  dataCompile();  //Compile All Data From Sensors
  if(millis() - msLastWrite >= msWriteRate) {
    if(writeToSD())
      msLastWrite = millis();
  }
  if(millis() - msLastRadio >= msRadioRate) {
    Serial1.println(dataString);  //Print Data To HC12 Radio
    msLastRadio = millis();
  }
}