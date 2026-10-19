/*SHC - The One Plane: Full Software Code; Collective of All Software For The Aircraft
  Created: 7 October 2026 | Modified: 19 October 2026 | By Arwyn & Seito
  Buzzer Notation Source: https://docs.arduino.cc/built-in-examples/digital/toneMelody/*/

//Libraries:
  #include <Wire.h> //Core I2C Communication
  #include <Adafruit_Sensor.h>  //General Altitude & Pressure Sensing
  #include "Adafruit_BMP3XX.h"  //BMP388 - Altitude & Pressure Sensing
  #include <Adafruit_BNO055.h>  //BNO055 - Motion & Orientation Sensing
  #include <utility/imumaths.h> //Motion & Orientation Sensing
  #include <SparkFun_u-blox_GNSS_Arduino_Library.h> //Location & GPS Tracking | http://librarymanager/All#SparkFun_u-blox_GNSS
  #include <SPI.h>  //Enables SPI Communication For Data Logging
  #include <SD.h> //Data Logging For SD Card

//Pins & Sensor Set-Up (  //HC12 Radio Does Not Need Configured Pins, But Pins Are: txPin 0 rxPin 1 HC12 Serial1):
  #define SDA 4 //I2C Manual Configuration
  #define SCL 5 //I2C Manual Configuration
  #define DOOR_PIN 17 //This Will Detect A Signal And Send A Signal To Buzzer
  #define chipSelect 20 //Only Neccessary Pin For SD Card | Other Pins: SD_SO 16 SD_CLK 18 SD_SI 19*/
  #define BUZZER_PIN 21 //Connect Buzzer
  #define LED_WHITE 22  //Connect Blinking LED
  SFE_UBLOX_GNSS myGNSS;  //Set-Up GNSS
  Adafruit_BMP3XX bmp;  //Set-Up BMP3XX
  Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); //Check I2C Device Address

//Global Variables:
  //String STATE = "PIRATE";
  bool onLED = false; //LED Starts As LOW (Off)
  //const int levelError = 5; //Meters Off The Ground While Still Classified As Ground Level
  //bool itFlew = false;  //Did The Aircraft Fly? (Did It Ever Enter FLIGHT?)
  bool songTime = true; //Did The Sand Deployment Doors Open? (Did The PICO Recieve Signal From Door Pin?)
  //Sensor Data (-999999 As Error-Handling Value):
    double storeTime = -999999;
    double groundLevel = -999999;
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
    String dataString_SD = ""; //String Used For SD Card Data Transmission
  //Conditional Cycles (Rather Than Delays, The Miliseconds Run Between Each Process Type):
    const int msBMPPullRate = 200;
    int msLastBMPPull = 0;
    const int msBNOPullRate = 200;
    int msLastBNOPull = 0;
    const int msGNSSPullRate = 500; //Higher Rate Between GNSS Data Pulls To Limit SPI Traffic
    int msLastGNSSPull = 0;
    const int msWriteRate = 200;
    int msLastWrite = 0;
    const int msRadioRate = 200;
    int msLastRadio = 0;
    const int msBlinkRate = 500;  //Required To Be 500ms
    int msLastBlink = 0;*/
  //Music (Songs: Mr. Sandman & Enter Sandman):
    float noteDuration = 0; //Length Of Note Playing
    int msLastNote = 0; //Time Since Last Note Played
    int noteSeq = 0;  //Current Note In Sequence; Returns To 0 For Repeats
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
      bool mrSandman = true;
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

void debugView(String debugType)  //Viewing Debug Messages
{
  if(debugType == "initPins")               Serial.println(F("{DEBUG - SETUP TEST: Initializing Pin Modes...}"));
  else if(debugType == "initPins_SUCCESS")    Serial.println(F("{DEBUG - SETUP SUCCESS: Pin Modes Initialized! :>}"));
  else if(debugType == "initBMP")             Serial.println(F("{DEBUG - SETUP TEST: Initializing BMP388 Sensor...}"));
  else if(debugType == "initBMP_ERROR")       Serial.println(F("{DEBUG - SETUP ERROR: Could Not Find Valid BMP388 Sensor; Check Wiring. :<)}"));
  else if(debugType == "initBMP_SUCCESS")     Serial.println(F("{DEBUG - SETUP SUCCESS: BMP388 Sensor Initialized! :>}"));
  else if(debugType == "initBNO")             Serial.println(F("{DEBUG - SETUP TEST: Initializing BNO055 Sensor...}"));
  else if(debugType == "initBNO_ERROR")       Serial.println(F("{DEBUG - SETUP ERROR: Could Not Find Valid BNO055 Sensor; Check Wiring & I2C ADDR. :<}"));
  else if(debugType == "initBNO_SUCCESS")     Serial.println(F("{DEBUG - SETUP SUCCESS: BNO055 Sensor Initialized! :>}"));
  else if(debugType == "initGNSS")            Serial.println(F("{DEBUG - SETUP TEST: Initializing GNSS...}"));
  else if(debugType == "initGNSS_ERROR")      Serial.println(F("{DEBUG - SETUP ERROR: GNSS Not Detected; Check Wiring. :<}"));
  else if(debugType == "initGNSS_SUCCESS")    Serial.println(F("{DEBUG - SETUP SUCCESS: GNSS Successfully Initialized! :>}"));
  else if(debugType == "initSD")              Serial.println(F("{DEBUG - SETUP TEST: Initalizing SD Card...}"));
  else if(debugType == "initSD_ERROR")        Serial.println(F("{DEBUG - SETUP ERROR: SD Card Initalization Failed. :<}"));
  else if(debugType == "initSD_SUCCESS")      Serial.println(F("{DEBUG - SETUP SUCCESS: SD Card Initialization Successful! :>}")); 
  else if(debugType == "headerSD")            Serial.println(F("{DEBUG - SETUP TEST: Opening File In SD Card & Writing Headers...}"));
  else if(debugType == "headerSD_SUCCESS")    Serial.println(F("{DEBUG - SETUP SUCCESS: Headers Successfully Written! :>}"));
  else if(debugType == "headerSD_ERROR")      Serial.println(F("{DEBUG - SETUP ERROR: Could Not Open File In SD Card To Write Headers. :<}"));
  else if(debugType == "initHC12")            Serial.println(F("{DEBUG - SETUP TEST: Initializing HC12 Serial Link...}"));
  else if(debugType == "initHC12_SUCCESS")    Serial.println(F("{DEBUG - SETUP ERROR: HC12 Serial Link Uninitialized. :<}"));
  else if(debugType == "initHC12_ERROR")      Serial.println(F("{DEBUG - SETUP SUCCESS: HC12 Serial Link Initialized! :>}"));
  else if(debugType == "blinkLED_ON")         Serial.println(F("{DEBUG - LOOP CHANGE: Switching LED State (Off -> On)}"));
  else if(debugType == "blinkLED_OFF")        Serial.println(F("{DEBUG - LOOP CHANGE: Switching LED State (On -> Off)}"));
  //else if(debugType == "playNote")            Serial.println(F("{DEBUG - LOOP CONDITION: Preparing To Play Current Buzzer Note...}"));
  else if(debugType == "playNote_CYCLE")      Serial.println(F("{DEBUG - LOOP CHANGE: Transitioned To Next Song!}")); 
  else if(debugType == "playNote_MrSand")    Serial.println("{DEBUG - LOOP CHANGE: Played Mr. Sandman Note " + String(noteSeq) + "(" + String (mrNotesArr[noteSeq]) + ") For " + String(noteDuration) + "ms...");
  else if(debugType == "playNote_EnterSand")  Serial.println("{DEBUG - LOOP CHANGE: Played Enter Sandman Note " + String(noteSeq) + "(" + String (etrNotesArr[noteSeq]) + ") For " + String(noteDuration) + "ms...");
  else if(debugType == "readBMP")             Serial.println(F("{DEBUG - LOOP CHANGE: Now Reading From BMP388...}"));
  else if(debugType == "readBMP_ERROR")       Serial.println(F("{DEBUG - LOOP ERROR: BMP388 Failed To Perform Reading. :<}"));
  else if(debugType == "readBMP_SUCCESS")     Serial.println(F("{DEBUG - LOOP SUCCESS: BMP388 Successfully Read!}"));
  //else if(debugType == "readBMP_DATA")        Serial.println("{DEBUG - LOOP DATA: BMP388 Data:\t|Temperature:" + String(temperature) + "*C|\t|Pressure:" + String(pressure) + "hPa|}");
  else if(debugType == "readBNO")             Serial.println(F("{DEBUG - LOOP CHANGE: Now Reading From BNO055...}"));
  else if(debugType == "readBNO_ERROR")       Serial.println(F("{DEBUG - LOOP ERROR: BNO055 Failed To Perform Reading. :<}"));
  else if(debugType == "readBNO_SUCCESS")     Serial.println(F("{DEBUG - LOOP SUCCESS: BNO055 Successfully Read!}"));
  //else if(debugType == "readBNO_orientDATA")  Serial.println("{DEBUG - LOOP DATA: BNO055 Orientation Data:\t|Yaw:" + String(yaw) + "|\t|Roll: " + String(roll) + "|\t|Pitch: " + String(pitch) +"|}");
  //else if(debugType == "readBNO_accelDATA")   Serial.println("{DEBUG - LOOP DATA: BNO055 Acceleration Data:\t|accelX:" + String(accelX) + "|\t|accelY: " + String(accelY) + "|\t|accelZ: " + String(accelZ) +"|}");
  else if(debugType == "readGNSS")            Serial.println(F("{DEBUG - LOOP CHANGE: Now Reading From GNSS...}"));
  else if(debugType == "readGNSS_ERROR")      Serial.println(F("{DEBUG - LOOP ERROR: GNSS Failed To Perform Reading. :<}"));
  else if(debugType == "readGNSS_SUCCESS")    Serial.println(F("{DEBUG - LOOP SUCCESS: GNSS Successfully Read!}"));
  //else if(debugType == "readGNSS_DATA")       Serial.println("{DEBUG - LOOP DATA: GNSS Data:\t|deciLat:" + String(decimalLat) + "|\t|deciLong:" + String(decimalLong) + "|\t|altitude:" + String(altitude) + "|\t|speed:" + String(speed) +"|}");
  //else if(debugType == "set_groundLevel")     Serial.println("{DEBUG - LOOP DATA: Ground Level Stored As: " + String(groundLevel) + "}");
  else if(debugType == "set_gL_ERROR")        Serial.println(F("{DEBUG - SETUP ERROR: Ground Level Error; Will Find When GNSS Read. :<}"));
  else if(debugType == "dataCompile")         Serial.println(F("{DEBUG - LOOP DATA: Compiling All Sensor Data...}"));
  //else if(debugType == "dataCompile_SUCCESS") Serial.println("{DEBUG - LOOP SUCCESS: All Data Compiled Into dataString: |" + dataString + "|}"); 
  //else if(debugType == "dataCompileSD")       Serial.println("{DEBUG - LOOP SUCCESS: dataStringSD Sotred As |" + dataString_SD + "|}"); 
  else if(debugType == "writeToSD")           Serial.println(F("{DEBUG - LOOP CHANGE: Attempting To Write dataStringSD To SD Card...}"));
  else if(debugType == "writeToSD_ERROR")     Serial.println(F("{DEBUG - LOOP ERROR: Error Opening datalog.csv. :<}"));
  else if(debugType == "writeToSD_SUCCESS")   Serial.println(F("{DEBUG - LOOP SUCCESS: dataString Successfully Written To The SD Card! :>}"));
  else if(debugType == "writeToHC12")         Serial.println(F("{DEBUG - LOOP TEST: Transmitting dataString To HC12 Radio...}"));
  else if(debugType == "SETUP")               Serial.println(F("{DEBUG - SETUP: Program Entered setup; Running Initialization Code...}"));
  else if(debugType == "SETUP_SUCCESS")       Serial.println(F("{DEBUG - SETUP SUCCESS: All Tests Complete; Control Passed To System Loop!! :D}"));
  else if(debugType == "beginBuzzer")         Serial.println(F("{DEBUG - LOOP CHANGE: Signal Recieved From Door Pin; Buzzer Begins To Play! :>}"));
  //else                                        Serial.println("{DEBUG - DEBUGVIEW: No debugView Option For \"" + debugType + "\"}");*/
}
void initPinModes() //Set-Up: Pins
{
  debugView("initPins");
  Serial.begin(9600); //Sets SD Card
  Serial1.begin(9600);  //Sets HC12 Radio; Other Pins: pinMode(rxPin, INPUT); pinMode(txPin, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);  //Determine Pin Type For Buzzer (OUTPUT - Higher Voltage)
  pinMode(LED_WHITE, OUTPUT); //Determine Pin Type For Blinking LED (OUTPUT - Higher Voltage)
  Wire.begin();
  Wire.setSDA(SDA);
  Wire.setSCL(SCL);
  debugView("initPins_SUCCESS");
}
void initBMP() //BMP388 Initialization & Error-Checking
{ 
  debugView("initBMP");
  if (!bmp.begin_I2C()) { //Testing Hardware I2C Mode
    debugView("initBMP_ERROR");
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  debugView("initBMP_SUCCESS");
}
void initBNO() //BNO055 Initialization & Error-Checking
{ 
  debugView("initBNO");
  if(!bno.begin()) {
    debugView("initBNO_ERROR");
  }
  debugView("initBNO_SUCCESS");
}
void initGNSS() //GNSS Initialization & Error-Checking
{
  debugView("initGNSS");
  if(!myGNSS.begin(Wire)) { //Connect To GNSS Using SPI Port
    debugView("initGNSS_ERROR");
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set I2C Port To Output UBX Protocol Data Only
  debugView("initGNSS_SUCCESS");
}
void initSD() //Initialize SD Card
{
  debugView("initSD");
  if(!SD.begin(chipSelect)) { //Checks If SD Card Initialized For Pin
    debugView("initSD_ERROR");
  }
  debugView("initSD_SUCCESS");
}
void headerSD() //Write SD Card File Headers; Organize Data
{
  debugView("headerSD");
  int fileNum = 1;
  while (true) {
    // Dynamically create a name like "flight_0.csv", "flight_1.csv", etc.
    sprintf(fileName, "datalog%d.csv", fileNum);
    
    // If this file number doesn't exist yet, we break out and use it!
    if (!SD.exists(fileName)) {
      break; 
    }
    fileNum++; // If it does exist, check the next number
  }
  File dataFile = SD.open(fileName, FILE_WRITE); //Open File (Only One File Can Open At A Time)
  if(dataFile) { //If File Is Available, Write To It
    dataFile.println("Time,Longitude,Latitude,Altitude,Pressure,Temperature,Roll,Pitch,Yaw,Ax,Ay,Az,Speed");
    dataFile.close();
    debugView("headerSD_SUCCESS");
  }
  else { //If File Cannot Open, Show Error
    debugView("headerSD_ERROR");
  }
}
void initHC12()
{
  debugView("initHC12");
  /*if(!HC12.isListening()) { //Commented Given Errors & Unnecessary
    debugView("initHC12_ERROR")
  }
  else {
    debugView("initHC12_SUCCESS");
  }*/
}
void blinkLED() //Blink The LED; Does Not Perform Time-Related Functions
{
  if(!onLED) {  //If LED Off, Turn On
    debugView("blinkLED_ON");
    digitalWrite(LED_WHITE, HIGH);
  }
  else {  //If LED On, Turn Off
    debugView("blinkLED_OFF");
    digitalWrite(LED_WHITE, LOW);
  }
  onLED = !onLED; //LED State Has Flipped, So Edit Accordingly!
}
void playNote() { //Play Current Note, And Prepare For The Next
  debugView("playNote");
  noTone(BUZZER_PIN); //Stop Playing On The Buzzer
  if(mrSandman) {
    if(noteSeq == mrSize) { //If All Notes Have Played, Onto The Next Song!
      noteSeq = 0;
      mrSandman = false;
    }
    noteDuration = (60.0 / float(mrBPM) / (float(mrDurations[noteSeq])/4.0) * 1000); //Duration(ms) Of Given Note
    debugView("playNote_MrSand");
    //tone(BUZZER_PIN, mrNotesArr[noteSeq], noteDuration);
  }
  else {
    if(noteSeq == etrSize) { //If All Notes Have Played, Onto The Next Song!
      noteSeq = 0;
      mrSandman = true;
    }
    noteDuration = (60.0 / float(etrBPM) / (float(etrDurations[noteSeq])/4.0) * 1000); //Duration(ms) Of Given Note
    debugView("playNote_EnterSand");
    //tone(BUZZER_PIN, etrNotesArr[noteSeq], noteDuration);
  }
  noteSeq++;
}
bool readBMP()  //Read Data From BMP388
{
  debugView("readBMP");
  if(!bmp.performReading()) { //Ensure BMP388 Reads Correctly
    debugView("readBMP_ERROR");
    return false;
  }
  debugView("readBMP_SUCCESS");
  temperature = bmp.temperature;
  pressure = bmp.pressure / 100.0;
  debugView("readBMP_DATA");
  return true;
}
bool readBNO()  //Read Data From BNO055
{
  debugView("readBNO");
  sensors_event_t orientationData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  if(orientationData.type == SENSOR_TYPE_ORIENTATION) {
    yaw = orientationData.orientation.x;       
    roll = orientationData.orientation.y; 
    pitch = orientationData.orientation.z; 
    debugView("readBNO_orientDATA");
  } 
  if(linearAccelData.type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    accelX = linearAccelData.acceleration.x;
    accelY = linearAccelData.acceleration.y;
    accelZ = linearAccelData.acceleration.z;
    debugView("readBNO_accelDATA");
  }
  debugView("readBNO_SUCCESS");
  return true;
}
bool readGNSS() //Read Data From GNSS
{
  debugView("readGNSS");
  if(myGNSS.getPVT() && myGNSS.getGnssFixOk()) {  //If GNSS Can Read Data
    long rawLat = myGNSS.getLatitude();
    long rawLong = myGNSS.getLongitude();
    decimalLat = rawLat / 10000000.0;
    decimalLong = rawLong / 10000000.0;
    altitude = myGNSS.getAltitudeMSL() / 1000.0; // Converts mm To Meters
    speed = (myGNSS.getGroundSpeed() / 1000.0) * 3.6; // Converts mm/s To km/h
    debugView("readGNSS_DATA");
    if(groundLevel < 0) { //If Ground Level Not Set, Set It
      groundLevel = myGNSS.getAltitudeMSL() / 1000.0;
      debugView("set_groundLevel");
    }
    debugView("readGNSS_SUCCESS");
    return true;
  }
  debugView("readGNSS_ERROR");
  return false;
}
void dataCompile()  //Compile All Sensor Data Into A String
{
  debugView("dataCompile");
  dataString = String(decimalLong,6) + ",";
  dataString += String(decimalLat,6) + ",";
  dataString += String(altitude,2) + ",";
  dataString += String(pressure,2) + ",";
  dataString += String(temperature,2) + ",";
  dataString += String(roll,2) + ",";
  dataString += String(pitch,2) + ",";
  dataString += String(yaw,2) + ",";
  dataString += String(accelX,2) + ",";
  dataString += String(accelY,2) + ",";
  dataString += String(accelZ,2) + ",";
  dataString += String(speed,2);
  debugView("dataCompile_SUCCESS");
  dataString_SD = String(storeTime) + "," + dataString;  //Add Time (And STATE) To SD Card DataString
  debugView("dataCompileSD_SUCCESS");
}
bool writeToSD()  //Send Data To SD Card
{
  debugView("writeToSD");
  File dataFile = SD.open(fileName, FILE_WRITE); //Open File (Only One File Can Open At A Time)
  if(dataFile) { //If File Is Available, Write To It
    dataFile.println(dataString_SD);
    dataFile.close();
    Serial.println(dataString_SD); //Print To Serial Port
    debugView("writeToSD_SUCCESS");
    return true;
  }
  debugView("writeToSD_ERROR");
  return false;
}
bool writeToHC12()
{
  debugView("writeToHC12");
  Serial1.println(dataString);  //Print To HC12 Radio
  return true;
}*/
void setup() {
  //STATE = "STANDBY"; 
  debugView("SETUP");
  initPinModes();
  initBMP();
  initBNO(); 
  initGNSS();
  if(myGNSS.getPVT() && myGNSS.getGnssFixOk()) {  //Attempt To Set Ground Level
    groundLevel = myGNSS.getAltitudeMSL() / 1000.0;
    debugView("set_groundLevel");
  }
  else {
    debugView("set_gL_ERROR");
  }
  initSD();
  headerSD();
  initHC12();
  debugView("SETUP_SUCCESS");
}

void loop() {
  
  //Aircraft States: //CAP: Below Ground Level, Above 300ft | Negative Time
    if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && !itFlew) //On Ground, But Has Not Yet Flown
      STATE = "LAUNCH";
    else if(altitude > groundLevel) //If In Air
      STATE = "FLIGHT";
    else if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && itFlew) //On Ground, And Has Flown
      STATE = "LANDED";
  //Blink LED:
    if(millis() - msLastBlink >= msBlinkRate) {
      blinkLED();
      msLastBlink = millis();
    }
  //Buzzer Melody:
    if(!songTime && digitalRead(DOOR_PIN)) {  //"digitalRead(DOOR_PIN)" Is An Attempt To Read From A PWM Signal From The Controller; Find Some Way To Read From The PWM, And Use That Instead. The Buzzer Code Works, And Should Start Playing Once Signal Is Recieved
      songTime = true;
      debugView("beginBuzzer");
    }
    if((songTime) && (millis() - msLastNote >= noteDuration * 1.30)) {
      playNote();
      msLastNote = millis();
    }
  //Output Sensor Data:
    storeTime = millis();
    if(millis() - msLastBMPPull >= msBMPPullRate) {
      if(readBMP())
        msLastBMPPull = millis();
    }
    if(millis() - msLastBNOPull >= msBNOPullRate) {
      if(readBNO())
        msLastBNOPull = millis();
    }
    if(millis() - msLastGNSSPull >= msGNSSPullRate) {
      if(readGNSS());
        msLastGNSSPull = millis();
    }
  //Send Data To SD Card & HC12 Radio:
    dataCompile();
    if(millis() - msLastWrite >= msWriteRate) {
      if(writeToSD())
        msLastWrite = millis();
    }
    if(millis() - msLastRadio >= msRadioRate) {
      if(writeToHC12())
        msLastRadio = millis();
    }
}