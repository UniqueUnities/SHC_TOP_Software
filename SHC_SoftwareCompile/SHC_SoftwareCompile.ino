/*SHC - The One Plane: Full Software Code; Collective of All Software For The Aircraft
  Created: 7 October 2026 | Modified: 18 October 2026 | By Arwyn & Seito
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

//Pins & Sensor Set-Up (REVIEW: Review Commented Pins | Implement Details Of Each Pin):
  //#define SDA 4 //I2C manual configuration
  //#define SCL 5 //I2C manual configuration
  #define txPin 0 //HC12 Radio
  #define rxPin 1 //HC12 Radio
  #define HC12 Serial1  //HC12 Radio
  //#define DOOR 17 //This Will Detect A Signal And Send A Signal To Buzzer
  //#define chipSelect 20 //Only Neccessary Pin For SD Card | Other Pins: SD_SO 16 SD_CLK 18 SD_SI 19
  //#define BUZZER_PIN 21 //Connect Buzzer
  #define LED_WHITE 22  //Connect Blinking LED
  //SFE_UBLOX_GNSS myGNSS;
  //Adafruit_BMP3XX bmp;
  //Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); //Check I2C Device Address

/*NoteVoltage.h (Voltage -> Notes):
  #define REST     0
  #define NOTE_C3  130.813
  #define NOTE_CS3 138.591
  #define NOTE_D3  146.832
  #define NOTE_DS3 155.563
  #define NOTE_E3  164.814
  #define NOTE_F3  174.614
  #define NOTE_FS3 184.997
  #define NOTE_G3  195.998
  #define NOTE_GS3 207.652
  #define NOTE_A3  220.000
  #define NOTE_AS3 233.082
  #define NOTE_B3  246.942
  #define NOTE_C4  261.626
  #define NOTE_CS4 277.183
  #define NOTE_D4  293.665
  #define NOTE_DS4 311.127
  #define NOTE_E4  329.628
  #define NOTE_F4  349.228
  #define NOTE_FS4 369.994
  #define NOTE_G4  391.995
  #define NOTE_GS4 415.305
  #define NOTE_A4  440.000
  #define NOTE_AS4 466.164
  #define NOTE_B4  493.883
  #define NOTE_C5  523.251
  #define NOTE_CS5 554.365
  #define NOTE_D5  587.330
  #define NOTE_DS5 622.254
  #define NOTE_E5  659.255
  #define NOTE_F5  698.456
  #define NOTE_FS5 739.989
  #define NOTE_G5  783.991
  #define NOTE_GS5 830.609
  #define NOTE_A5  880.000
  #define NOTE_AS5 932.328
  #define NOTE_B5  987.767*/
  
//Aircraft States:
  /*enum piracyStates {
    PIRATE = 0;
    STANDBY;
    LAUNCH;
    FLIGHT;
    LANDED;
  }*/

//Global Variables:
  //enum piracyStates State = PIRATE;
  //bool state = false; //uart
  int stateLED = LOW; //LED Starts As LOW (Off)
  //const int levelError = 5; //Feet Off The Ground While Still Classified As Ground Level
  //bool itFlew = false;  //Aircraft Flew
  //bool musicTime = false; //Sand Deployment Doors Opened
  //Sensor Data:
    //const float SEALEVELPRESSURE_HPA 1013.25 //never used
<<<<<<< HEAD
    double = storeTime = -1;
    double groundLevel = -1;
    double temperature = -1;
    double pressure = -1;
    double altitude = -1;
    double decimalLat = -1;
    double decimalLong = -1;
    double roll = -1;
    double pitch = -1;
    double yaw = -1;
    double accelX = -1;
    double accelY = -1;
    double accelZ = -1;
    double speed = -1;
=======
    double = storeTime = -9999999;
    double groundLevel = -9999999;
    double temperature = -9999999;
    double pressure = -9999999;
    double altitude = -9999999;
    double decimalLat = -9999999;
    double decimalLong = -9999999;
    double roll = -9999999;
    double pitch = -9999999;
    double yaw = -9999999;
    double accelX = -9999999;
    double accelY = -9999999;
    double accelZ = -9999999;
    double speed = -9999999;
>>>>>>> c38c4a316aed61b0cb48102dd8e31057a69aaa05
    String dataString = "";
  //Conditional Cycles:
    /*const int msBMPPullRate = 100; //Time Between BMP388 Data Pulls
    int msLastBMPPull = 0;
    const int msBNOPullRate = 100; //Time Between BNO055 Data Pulls
    int msLastBNOPull = 0;
    const int msGNSSPullRate = 1000;  //Higher Rate Between GNSS Data Pulls To Limit SPI Traffic
    int msLastGNSSPull = 0;
    const int msWriteRate = 100;  //SD Card Write Rate
    int msLastWrite = 0;*/
    const int msRadioRate = 100;  //HC12 Radio Transmission Rate
    int msLastRadio = 0;
    const int msBlinkRate = 500;  //LED Blinking Rate
    int msLastBlink = 0;
    //float noteDuration = 0; //Length Of Note Playing
    //int msLastNote = 0; //Time Since Last Note Played
    //int noteSeq = 0;  //Current Note In Sequence; Returns To 0 For Repeats
  //Music - //Mr. Sandman:
    //bool mrSandman = true;
    //const int mrBPM = 142;  //Mr. Sandman BPM
    /*const float mrNotesArr[] {
      NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4,NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_A3, NOTE_E3, NOTE_E3, NOTE_D3, NOTE_E3, NOTE_G3, REST, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_DS4, NOTE_DS4, NOTE_E4, NOTE_D4, REST, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, REST, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, NOTE_D3, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, REST, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_C5, REST
    };
    /*const float mrDurations[] {
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 4, 8, 2, 8, 8, 8, 4, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 4, 4, 8, 4, 8, 8, 4, 8, 2, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 8, 4, 8, 8, 8, 2, 4, 8, 8, 4, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 1, 8, 0.25
    };
    const int mrSize = sizeof(mrNotesArr) / sizeof(int);
  //Music - Enter Sandman:
    const int etrBPM = 130; //Enter Sandman BPM
    const float etrNotesArr[] {
      NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_G3, REST, NOTE_E3, REST
    }
    const float etrDurations[] {
      4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 8, 2, 0.25
    }
    const int etrSize = sizeof(etrNotesArr) / sizeof(int);*/

void availSerial() //Wait For Serial Monitor Availability (REVIEW: Is This Needed?):
{
  Serial.println(F("{DEBUG - SETUP TEST: Testing Serial Monitor Availability...}"));
  while(!Serial) {  //Wait For Serial Port To Open In Terminal; Needed For Native USB Port Only
    Serial.println(F("{DEBUG - SETUP ERROR: Serial Monitor Not Yet Available; Testing Again...}"));
    delay(10);
  }
  Serial.println(F("{DEBUG - SETUP Success: Serial Monitor Available! :>}"));
}
void initPinModes() //Pins Set-Up
{
  Serial.println(F("{DEBUG - SETUP TEST: Initializing Pin Modes...}"));
  Serial.begin(115200); //REVIEW: Why 115200?
  //pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); //Turn-on All Non-White LEDs
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  //Wire.begin();
  //Set-Up SDA & SCL Wires (REVIEW: Is This Needed?):
  //Serial.println("{DEBUG - TEST: Initializing SDA & SCL Wires...}");
  //Wire.setSDA(SDA);
  //Wire.setSCL(SCL);
  //Serial.println("{DEBUG - SUCCESS: Set-Up SDA & SCL Wires! :>}");
  Serial.println(F("{DEBUG - SETUP SUCCESS: Pin Modes Initialized! :>}"));
}
/*void initBMP() //BMP388 Initialization & Error-Checking
{ 
  Serial.println(F("{DEBUG - SETUP TEST: Initializing BMP388 Sensor...}"));
  if (!bmp.begin_I2C()) { //Testing Hardware I2C Mode
    Serial.println(F("{DEBUG - SETUP ERROR: Could Not Find Valid BMP388 Sensor; Check Wiring. :<)}"));
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  Serial.println(F("{DEBUG - SETUP SUCCESS: BMP388 Sensor Initialized! :>}"));
}
void initBNO() //BNO055 Initialization & Error-Checking
{ 
  Serial.println(F("{DEBUG - SETUP TEST: Initializing BNO055 Sensor...}"));
  if(!bno.begin()) {
    Serial.print(F("{DEBUG - SETUP ERROR: Could Not Find Valid BNO055 Sensor; Check Wiring & I2C ADDR. :<}"));
  }
  Serial.println(F("{DEBUG - SETUP SUCCESS: BNO055 Sensor Initialized! :>}"));
}
void initGNSS() //GNSS Initialization & Error-Checking
{
  Serial.println(F("{DEBUG - SETUP TEST: Initializing GNSS...}"));
  //myGNSS.enableDebugging(); //Show Debug Messages On Serial
  if(!myGNSS.begin(Wire)) { //Connect To GNSS Using SPI Port, csPin, & Speed Setting
    Serial.println(F("{DEBUG - SETUP ERROR: GNSS Not Detected; Check Wiring. :<}"));
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set I2C Port To Output UBX Protocol Data Only
  Serial.println(F("{DEBUG - SETUP SUCCESS: GNSS Successfully Initialized! :>}"));
}*/
/*void initSD() //Initialize SD Card
{
  Serial.println(F("{DEBUG - SETUP TEST: Initalizing SD Card...}"));
<<<<<<< HEAD
  if(!SD.begin(chipSelect)) {
    Serial.println(F("{DEBUG - SETUP ERROR: SD Card Initalization Failed. :<}"));
  }
=======
  /*SD Card Initalization (REVIEW: Should This Be Commented?):
    bool sdInitialized = false;
    if(RP_CLK_GPIO >= 0) { //No Special Requirements On Pin Locations; PIO Programmed
      sdInitialized = SD.begin(RP_CLK_GPIO, RP_CMD_GPIO, RP_DAT0_GPIO);
    }
    else { //Ensure SPI Pinout SD Card Connected To Is Configured Properly & Select Correct SPI From _MISO Pin For RP2040
      if(_MISO == 0 || _MISO == 4 || _MISO == 16) {
        SPI.setRX(_MISO);
        SPI.setTX(_MOSI);
        SPI.setSCK(_SCK);
        sdInitialized = SD.begin(_CS);
      }
      else if (_MISO == 8 || _MISO == 12) {
        SPI1.setRX(_MISO);
        SPI1.setTX(_MOSI);
        SPI1.setSCK(_SCK);
        sdInitialized = SD.begin(_CS, SPI1);
      }
      else {
        Serial.println(F("ERROR: Unknown SPI Configuration"));
        return;
      }*/

  if(!SD.begin(chipSelect)) {
    Serial.println(F("{DEBUG - SETUP ERROR: SD Card Initalization Failed. :<}"));
  }
    /*Determine If Card Present (REVIEW: Should This Be Commented?):
      if (!sdInitialized) {
        Serial.println("{DEBUG - SETUP ERROR: Card Failed Or Not Present. :<}");
        return; //End Set-Up; Nothing Can Be Stored | REVIEW: Should We Implement A Loop To Ensure Proper Set-Up?
      }*/
>>>>>>> c38c4a316aed61b0cb48102dd8e31057a69aaa05
  Serial.println(F("{DEBUG - SETUP SUCCESS: SD Card Initialization Successful! :>}")); 
  File dataFile = SD.open("datalog.csv", FILE_WRITE); //Open File (Only One File Can Open At A Time)
  dataString="Time,Longitude,Latitude,Altitude,Pressure,Temperature,Roll,Pitch,Yaw,Ax,Ay,Az,Speed";
  if(dataFile) { //If File Is Available, Write To It
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString); //Print To Serial Port
    Serial.println("{DEBUG - Headers Successfully Written! :>}");
  }
  else { //If File Is Not Open, Show Error
    Serial.println("{DEBUG - LOOP ERROR: Error Opening datalog.csv. :<}");
  }
<<<<<<< HEAD
}*/
=======
}
>>>>>>> c38c4a316aed61b0cb48102dd8e31057a69aaa05
void initHC12()
{
  Serial.println(F("{DEBUG - SETUP TEST: Initializing HC12 Serial Link...}"));
  HC12.begin(9600);
  /*if(!HC12.isListening()) {
    Serial.println(F("{DEBUG - SETUP ERROR: HC12 Serial Link Uninitialized. :<}"));
  }
  else {
    Serial.println(F("{DEBUG - SETUP SUCCESS: HC12 Serial Link Initialized! :>}"));
  }*/
  Serial.println(F("{DEBUG - SETUP SUCCESS: HC12 Serial Link Initialized! :>}"));
}
/*void blinkLED() //Blink The LED; Does Not Perform Time-Related Functions
{
  if(stateLED = LOW) {
    stateLED = HIGH;
    Serial.println("Switching LED State (Off -> On)}");
  }
  else {
    stateLED = LOW;
    Serial.println("Switching LED State (On -> Off)}");
  }
  digitalWrite(LED_WHITE, stateLED)
}

void playNote() { //Play Current Note, And Prepare For The Next
  if(noteSeq == size) {
    noteSeq = 0;
    mrSandman = !mrSandman;
  }
  noTone(BUZZER_PIN); //Stop Playing On The Buzzer
  if(mrSandman) {
    noteDuration = 60 / mrBPM / (mrDurations[mrNoteSeq]/4); //Duration(sec) Of Given Note
    tone(BUZZER_PIN, notesArr[mrNoteSeq], noteDuration);
  }
  else {
    noteDuration = 60 / etrBPM / (etrDurations[etrNoteSeq]/4); //Duration(sec) Of Given Note
    tone(BUZZER_PIN, notesArr[etrNoteSeq], noteDuration);
  }
  noteSeq++;
}*/
/*bool readBMP355()
{
  if(!bmp.performReading()) {
      Serial.println(F("{DEBUG - LOOP ERROR: BMP388 Failed To Perform Reading. :<}"));
      return false;
    }

    Serial.println(F("{DEBUG - LOOP SUCCESS: BMP388 Successfully Read; Now Outputting Data:}"));
    temperature = bmp.temperature;
    Serial.println("Temperature: " + String(temperature) + "*C");
    pressure = bmp.pressure / 100.0;
    Serial.println("Pressure: " + String(pressure) + "hPa");
    //altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    //Serial.print("Approx. Altitude: " + String(altitude)) + "m");
    Serial.println(F("{DEBUG - LOOP SUCCESS: BMP388 Data Outputted!}"));
}
bool readBNO055()
{
    Serial.println(F("{DEBUG - LOOP CHANGE: Now Reading From BNO055...}"));
    //REVIEW: Implement Way To Verify Errorless Data Pull?
    sensors_event_t orientationData, linearAccelData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    if(orientationData.type == SENSOR_TYPE_ORIENTATION) {
      Serial.print("Orientation:");
      yaw = orientationData.orientation.x;       
      Serial.print("\tyaw: " + String(yaw));
      roll = orientationData.orientation.y; 
      Serial.print(" |\troll: " + String(roll));
      pitch = orientationData.orientation.z; 
      Serial.print(" |\tpitch: " + String(pitch));
    } 

  if(linearAccelData.type == SENSOR_TYPE_LINEAR_ACCELERATION) {
      Serial.print("Linear:");
      accelX = linearAccelData.acceleration.x;
      Serial.print("\taccelX: " + String(accelX));
      accelY = linearAccelData.acceleration.y;
      Serial.print(" |\taccelY: " + String(accelY)));
      accelZ = linearAccelData.acceleration.z;
      Serial.print(" |\taccelZ: " + String(accelZ));
    }
    /*Additional Data Points (REVIEW: Either Include Or Remove):
      bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
      bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
      bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
      bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);*/

      /*int8_t boardTemp = bno.getTemp();
      Serial.println("\nBoard Temperature: " + String(boardTemp));
      Serial.println(F("{DEBUG - LOOP READING: Reading BNO055 Calibration Data:}"));
      uint8_t system, gyro, accel, mag = 0;
        bno.getCalibration(&system, &gyro, &accel, &mag);
      Serial.println("Calibration:");
      Serial.println("Sys = " + String(system));
      Serial.println("Gyro = " + String(gyro));
      Serial.println("Accel = " + String(accel));
      Serial.println("Mag = " + String(mag));*/
    //return true;
//}
/*bool readGNSS()
{
<<<<<<< HEAD
  if(myGNSS.getPVT() && myGNSS.getGnssFixOk()) {
    long rawLat = myGNSS.getLatitude();
    long rawLong = myGNSS.getLongitude();
    decimalLat = rawLat / 10000000.0;
    decimalLong = rawLong / 10000000.0;
    altitude = myGNSS.getAltitudeMSL() / 1000.0; // Converts mm to Meters
    speed = (myGNSS.getGroundSpeed() / 1000.0) * 3.6; // Converts mm/s to km/h
    if(groundLevel < 0) {
      groundLevel = myGNSS.getAltitudeMSL() / 1000.0;
=======
  if(myGNSS.getPVT() == true) {
    if(myGNSS.getGnssFixOk() == true) {
      //REVIEW: Implement System To Check For Reading Errors
      long rawLat = myGNSS.getLatitude();
      long rawLong = myGNSS.getLongitude();
      decimalLat = rawLat / 10000000.0;
      decimalLong = rawLong / 10000000.0;
      altitude = myGNSS.getAltitudeMSL() / 1000.0; // Converts mm to Meters
      speed = (myGNSS.getGroundSpeed() / 1000.0) * 3.6; // Converts mm/s to km/h
      //byte SIV = myGNSS.getSIV();
      Serial.println("Lat: " + String(decimalLat,7));
      Serial.println("Long: " + String(decimalLong,7));
      Serial.println("Alt (MSL): " + String(altitude) + "m");
      Serial.println("Speed: " + String(speed) + "(km/h)");
      return true;
>>>>>>> c38c4a316aed61b0cb48102dd8e31057a69aaa05
    }
    //byte SIV = myGNSS.getSIV();
    Serial.println("Lat: " + String(decimalLat,7));
    Serial.println("Long: " + String(decimalLong,7));
    Serial.println("Alt (MSL): " + String(altitude) + "m");
    Serial.println("Speed: " + String(speed) + "(km/h)");
    return true;
  }
  return false;
}*/
void dataCompile()
{
  dataString = String(storeTime) + ",";
  dataString += String(decimalLong,6) + ",";
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
}
/*bool writeToSD()
{
  Serial.println("{DEBUG - LOOP CHANGE: Data Compiled; Now Writing In SD Card...}");
  dataCompile();
  File dataFile = SD.open("datalog.csv", FILE_WRITE); //Open File (Only One File Can Open At A Time)
  if(dataFile) { //If File Is Available, Write To It
    msLastWrite = millis();
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString); //Print To Serial Port
    Serial.println("{DEBUG - LOOP SUCCESS: Data Successfully Written! :>}");
    return true;
  }
  else { //If File Is Not Open, Show Error
    Serial.println("{DEBUG - LOOP ERROR: Error Opening datalog.csv. :<}");
  }
  return false;
}*/
bool writeToHC12()
{
    //digitalWrite(LED_BUILTIN, state); //visual indicator
    dataCompile();
    HC12.println(dataString);
    msLastRadio = millis();
    Serial.println("{DEBUG - LOOP SUCCESS: Data Successfully Transmitted To HC12 Radio! :>}");
    //state != state;
    /*Recieve From Ground Station:
      if(HC12.available() > 0) {
      Serial.print(millis());
      Serial.print(" RF: ");
      while (HC12.available() > 0) {
        Serial.print(char(HC12.read()));
      }
    }*/
    return true;
}

void setup() {
  //piracyStates STATE = STANDBY;
  availSerial();
  initPinModes();
  //initBMP();
  //initBNO(); 
  //initGNSS();
  /*if(myGNSS.getPVT() && myGNSS.getGnssFixOk()) {
    groundLevel = myGNSS.getAltitudeMSL() / 1000.0;
    Serial.println(F("{DEBUG - SETUP SUCCESS: Ground Level Stored As" + String(groundLevel) + "! :>}"));
  }
  else {
    Serial.println(F("{DEBUG - SETUP ERROR: Ground Level Error; Will Find When GNSS Read. :<}"));
  }*/
  //initSD();
  initHC12();
  Serial.println(F("{DEBUG - SETUP SUCCESS: All Tests Complete; Control Passed To System Loop!! :D}"));
}

void loop() {
  /*Blink LED:
    if(millis() - msLastBlink >= msBlinkRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastBlink) + "ms >= " + String(msBlinkRate) + "ms.; ");
      blinkLED();
      msLastBlink = millis();
    }
  //Buzzer Melody (REVIEW: If Signal Recieved From Pin):
      //Serial.println("{DEBUG - LOOP CALC: Buzzer Notes (" + String(millis() - msLastNote()) + "ms >= " + String(duration * 1.30) + "ms)}");
      if(digitalRead(DOOR_PIN))
        songTime = true;
      if((songTime) && (millis() - msLastNote >= noteDuration * 1.30)) {
        playNote();
        msLastNote = millis();
      }*/
  /*Output Sensor Data:
    storeTime = millis();
    Serial.println("Current Time: " + String(storeTime) + "ms");
    if(millis() - msLastBMPPull >= msBMPPullRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastBMPPull) + "ms >= " + String(msBMPPullRate) + "ms; Pulling BMP388 Sensor Data...}");
      if(readBMP())
        msLastBMPPull = millis();
    }
    if(millis() - msLastBN0Pull >= msBNOPullRate) {
        Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastBNOPull) + "ms >= " + String(msBNOPullRate) + "ms; Pulling BNO055 Sensor Data...}");
        if(readBNO)
          msLastBNOPull = millis();
    }
    if(millis() - msLastGNSSPull >= msGNSSPullRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastGNSSPull) + "ms >= " + String(msGNSSPullRate) + "ms; Pulling Data...}");
      if(readGNSS());
        msLastGNSSPull = millis();
    }
  Serial.println(F("{DEBUG - LOOP SUCCESS: All Sensor Data Successfully Read! :>}"));*/

  /*SD Card Storage (REVIEW: Implement Beyond Tests):
    if(millis() - msLastWrite >= msWriteRate) { //Review: Can We Open The File Before Compiling Data To Send; It Would Be Optimal?
      Serial.println("{DEBUG - LOOP CHANGE: Preparing Data Transfer To SD Card...}");
      if(writeToSD())
        msLastWrite = millis();
  }*/
  //HC12 Radio Transmission (REVIEW: Implement Past Test):
    if(millis() - msLastRadio >= msRadioRate) {
      Serial.println("{DEBUG - LOOP CHANGE: Preparing Data Transfer To HC12 Radio...}");
      if(writeToHC12())
        msLastRadio = millis();
    }
  /*CAP: Below Ground Level, Above 300ft
  //CAP: Negative Time
  switch(STATE) { //State Machine
    case 0: //PIRATE
      if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && !itFlew)
        STATE = LAUNCH;
      else if(altitude > groundLevel)
        STATE = FLIGHT;
      else if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && itFlew)
        STATE = LANDED;
      break;
    case 1: //STANDBY
      if() {  //REVIEW: Signal To Move
        STATE = LAUNCH;
      }
      break;
    case 2: //LAUNCH
      if(altitude > groundLevel + levelError)
        STATE = FLIGHT;
        itFlew = true;
      }
      break;
    case 3: //FLIGHT
      if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && itFlew) { //REVIEW: Finished Flight; Altitude == Ground Level
        STATE = LANDED;
      }
      break;
    case 4: //LANDED
      break;
    default:
      if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && !itFlew)
        STATE = LAUNCH;
      else if(altitude > groundLevel)
        STATE = FLIGHT;
      else if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && itFlew)
        STATE = LANDED;
      break;
  }*/
}