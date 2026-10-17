/*Documentation (REVIEW: Add Libraries | Ensure Correct Information):
  SHC - The One Plane: Full Software Code; Collective of All Software For The Aircraft
  Created: 7 October 2026
  Modified: 15 October 2026
  By Arwyn & Seito

  Uses Adafruit BMP3xx Library.
    Sensor Uses I2C or SPI For Communication; 2/'4 Pins Required
  Uses Adafruit Unified Sensor Library (Adafruit_Sensor) To Provide Common "Type" For Sensor Data
    Need To Dowload Adafruit_Sensor Library To Run Code
    Assigned Unique ID (12345) To Sensor For Identification In DataLogs
  Queeries U-Blox Module For Lat & Long & Altitude Over SPI; Also Turns Off NMEA Output On SPI Port TO Decrease SPI Traffic
    Note: To Convert Lat/Long To Coordinates, Divide Numbers By 10,000,000
    DSEL/SPI Jumper Closed On U-Blox Board Must Be Soldered For SPI Communication
  SD Card Logger Logs Data From Sensors
    Analog Sensors On Analog ins 0, 1, & 2
    SD Card Attached to SPI Bus: MISO - Pin4 | MOSI - Pin7 | CS - Pin5 | SCK - Pin6
  Notation Source: https://docs.arduino.cc/built-in-examples/digital/toneMelody/*/

//Libraries (REVIEW: Why Are Each Included?):
  #include <Wire.h> //core I2C communication
  #include <Adafruit_Sensor.h> //altitude & pressure sensing
  #include "Adafruit_BMP3XX.h" //altitude & pressure sensing specifically for the bmp388
  #include <Adafruit_BNO055.h> //motion & orientation sensing specifically for the bno055
  #include <utility/imumaths.h> //motion & orientation sensing
  #include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS location & GPS tracking
  #include <SPI.h> //enables SPI communication for data logging
  #include <SD.h> //data logging for the SD card

//Pins & Sensor Set-Up (REVIEW: Review Commented Pins | Implement Details Of Each Pin):
  //#define SDA 4
  //#define SLC 5
  #define txPin 0 //HC12 Radio
  #define rxPin 1 //HC12 Radio
  #define HC12 Serial1  //HC12 Radio
  #define chipSelect 20 //SD Card
  #define BUZZER_PIN 21 //Connect Buzzer
  #define LED_WHITE 22  //Connect Blinking LED
  /*Declare Pins For SPI - Do Not Need To Configure GPIO Pins When Using I2C
    #define BMP_SCK 13
    #define BMP_MISO 12
    #define BMP_MOSI 11
    #define BMP_CS 10
    Instantiate Instance of SPI Class: #define spiPort SPI //Standard SPI Port*/
  SFE_UBLOX_GNSS myGNSS;
  Adafruit_BMP3XX bmp;
  Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); //Check I2C Device Address

//NoteVoltage.h (Voltage -> Notes):
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
  #define NOTE_B5  987.767
  
//Aircraft States:
  /*enum piracyStates {
    PIRATE = 0;
    STANDBY;
    LAUNCH;
    FLIGHT;
    LANDED;
    CAPTAIN;
  }*/

//Global Variables (REVIEW: Ensure Descriptive Names & Implement Comments):
  enum piracyStates State = PIRATE;
  const float SEALEVELPRESSURE_HPA 1013.25  //REVIEW: Can This Be A Const Float?
  int groundLevel = 0;  //REVIEW: Set Ground Level
  const int levelError = 5; //REVIEW: Error Requires Improved Estimate
  const int msPullRate = 100; //Time Between Sensor Data Pulls
  int msLastPull = 0; //Sensor Data Pulls
  const int msGNSSPullRate = 1000;  //Higher Rate Between GNSS Data Pulls To Limit SPI Traffic
  int msLastGNSSPull = 0; //GNSS Data Pulls
  const int msWriteRate = 100;
  int msLastWrite = 0;
  const int msRadioRate = 100;
  int msLastRadio = 0;
  const int msBlinkRate = 500;  //LED Blinking Rate
  int msLastBlink = 0;  //LED Blinking
  int stateLED = LOW;
  int msLastNote = 0; //Buzzer Melody
  float duration = 0; //Duration Of Buzzer Notes
  bool buzzerSounded = false; //Ensures Buzzer Only Plays Once
  bool itFlew = false;  //Aircraft Flew
  long baud = 9600; //uart
  bool state = false; //uart
  long rawLat=0;
  long rawLong=0;
  double decimalLat=0;
  double decimalLong=0;
  double altitude=0;
  double pressure=0;
  double temp=0;
  double roll=0;
  double pitch=0;
  double yaw=0;
  double ax=0;
  double ay=0;
  double az=0;
  double speed=0;
  const int BPM = 142;  //Mr. Sandman BPM
  float notesArr[] {  //Mr. Sandman Notes
    NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_A3, NOTE_E3, NOTE_E3, NOTE_D3, NOTE_E3, NOTE_G3, REST, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_DS4, NOTE_DS4, NOTE_E4, NOTE_D4, REST, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, REST, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, NOTE_D3, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, REST, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_C5
  };
  int size = sizeof(notesArr) / sizeof(int);  //Length Of Notes Array
  int noteSeq = 0;  //Current Note In Sequence
  float durations[] { //Mr. Sandman Not Durations
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 4, 8, 2, 8, 8, 8, 4, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 4, 4, 8, 4, 8, 8, 4, 8, 2, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 8, 4, 8, 8, 8, 2, 4, 8, 8, 4, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 1, 8
  };

void availSerial() //Wait For Serial Monitor Availability (REVIEW: Is This Needed?):
{
  Serial.println(F("{DEBUG - SETUP TEST: Testing Serial Monitor Availability...}"));
  while(!Serial) { //Wait For Serial Port To Open In Terminal; Needed For Native USB Port Only
    Serial.println(F("{DEBUG - SETUP ERROR: Serial Monitor Not Yet Available; Testing Again. :<)}"));
    delay(10);
  }
  Serial.println(F("{DEBUG - SETUP Success: Serial Monitor Available! :>}"));
}

void initPinModes() //Initialize Pin Modes
{
  Serial.println(F("{DEBUG - SETUP TEST: Initializing Pin Modes...}"));
  Serial.begin(115200); //REVIEW: Why 115200?
  pinMode(BUZZER_PIN, OUTPUT);  //Define Pin Mode For The Buzzer
  pinMode(LED_BUILTIN, OUTPUT); //Define Pin Mode For Blinking LED
  pinMode(rxPin, INPUT);  //Define Pin Mode For HC12 rx
  pinMode(txPin, OUTPUT); //Define Pin Mode For HC12 tx
  Serial.begin(baud); //REVIEW: What Does This Do?
  //spiPort.begin();
  Wire.begin();
  /*Set-Up SDA & SCL Wires (REVIEW: Is This Needed?):
    Serial.println("{DEBUG - TEST: Initializing SDA & SCL Wires...}");
    Wire.setSDA(SDA);
    Wire.setSCL(SCL);
    Serial.println("{DEBUG - SUCCESS: Set-Up SDA & SCL Wires! :>}");*/
  Serial.println(F("{DEBUG - SETUP SUCCESS: Pin Modes Initialized! :>}"));
}

void initBMP() //Initialize BMP388
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

void initBNO() //Initialize BNO055 
{ 
  Serial.println(F("{DEBUG - SETUP TEST: Initializing BNO055 Sensor...}"));
  if(!bno.begin()) {
    Serial.print(F("{DEBUG - SETUP ERROR: Could Not Find Valid BNO055 Sensor; Check Wiring & I2C ADDR. :<}"));
  }
  Serial.println(F("{DEBUG - SETUP SUCCESS: BNO055 Sensor Initialized! :>}"));
}

void initGNSS() //Initialize GNSS
{
  Serial.println(F("{DEBUG - SETUP TEST: Initializing GNSS...}"));
  //myGNSS.enableDebugging(); //Show Debug Messages On Serial
  if(!myGNSS.begin(Wire)) { //Connect To GNSS Using SPI Port, csPin, & Speed Setting
    Serial.println(F("{DEBUG - SETUP ERROR: GNSS Not Detected; Check Wiring. :<}"));
  }
  //myGNSS.factoryDefault(); delay(5000); //Reset GNSS To Factory Defaults
  //myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //Set SPI Port To Output UBX Only (Turn Off NMEA Noise)
  //myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);  //Save Only Communication Port Settings To Flsh And BBR
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set I2C Port To Output UBX Protocol Data Only
  Serial.println(F("{DEBUG - SETUP SUCCESS: GNSS Successfully Initialized! :>}"));
}

void initSD() //Initialize SD Card
{
  Serial.println(F("{DEBUG - SETUP TEST: Initalizing SD Card...}"));
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
    Serial.println(F("{DEBUG - SETUP SUCCESS: SD Card Initialization Successful! :>}"));  
}

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

void blinkLED() //Blink The LED; Does Not Perform Time-Related Functions
{
  if(stateLED = LOW) {
    stateLED = HIGH;
    Serial.println("; Switching LED State (Off -> On)}");
  }
  else {
    stateLED = LOW;
    Serial.println("; Switching LED State (On -> Off)}");
  }
  digitalWrite(LED_WHITE, stateLED)
}

bool readSensors()  //Read All Sensor Data
{
    Serial.println("Current Time: " + String(millis()) + "ms");
    if(!bmp.performReading()) { //Review (Test): Implement System To Redo Reading If Error
      Serial.println(F("{DEBUG - LOOP ERROR: BMP388 Failed To Perform Reading. :<}"));
      return false;
    }

    Serial.println(F("{DEBUG - LOOP SUCCESS: BMP388 Successfully Read; Now Outputting Data:}"));
    temp = bmp.temperature;
    pressure = bmp.pressure / 100.0;
    Serial.println("Temperature: " + String(bmp.temperature) + "*C");
    Serial.println("Pressure: " + String(bmp.pressure / 100.0) + "hPa");
    //Serial.print("Approx. Altitude: " + String(bmp.readAltitude(SEALEVELPRESSURE_HPA)) + "m");
    Serial.println(F("{DEBUG - LOOP READING: BMP388 Read, Now Reading From BNO055...}"));

    //REVIEW: Implement Way To Verify Errorless Data Pull?
    sensors_event_t orientationData, linearAccelData; //Additional Data Points: angVelocityData, accelerometerData, gravityData, magnetometerData
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    if (orientationData.type == SENSOR_TYPE_ORIENTATION) {
      Serial.print("Orient:");
      yaw   = orientationData.orientation.x; 
      roll  = orientationData.orientation.y; 
      pitch = orientationData.orientation.z; 
      Serial.print("\tyaw= ");   Serial.print(yaw);
      Serial.print(" |\troll= ");  Serial.print(roll);
      Serial.print(" |\tpitch= "); Serial.println(pitch);
    } 

  if(linearAccelData.type == SENSOR_TYPE_LINEAR_ACCELERATION) {
      Serial.print("Linear:");
      ax = linearAccelData.acceleration.x;
      ay = linearAccelData.acceleration.y;
      az = linearAccelData.acceleration.z;
      Serial.print("\tAx= ");   Serial.print(ax);
      Serial.print(" |\tAy= ");  Serial.print(ay);
      Serial.print(" |\tAz= ");  Serial.println(az);
    }
    /*Additional Data Points (REVIEW: Either Include Or Remove):
      bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
      bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
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
    }   
    //Serial.println("{DEBUG - LOOP CALC: Time Since Last GNSS Data Pull " + String(millis()-msLastGNSSPull) + "ms}");
    if(millis() - msLastGNSSPull >= msGNSSPullRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastGNSSPull) + "ms >= " + String(msGNSSPullRate) + "ms; Pulling Data...}");
      //REVIEW: Implement System To Check For Reading Errors
      msLastGNSSPull = millis();
      //fetch raw scaling factors (Degrees * 10^-7)
      rawLat = myGNSS.getLatitude();
      rawLong = myGNSS.getLongitude();
      //convert to full 64-bit floating-point decimal degrees
      decimalLat = rawLat / 10000000.0;
      decimalLong = rawLong / 10000000.0;

      alt = myGNSS.getAltitude() / 1000.0; // Converts mm to Meters
      speed = (myGNSS.getGroundSpeed() / 1000.0) * 3.6; // Converts mm/s to km/h
      byte SIV = myGNSS.getSIV();
      Serial.println("Lat: " + String(decimalLat,7));
      Serial.println("Long: " + String(decimalLong,7));
      Serial.println("Alt: " + String(alt) + "m");
      Serial.println("Speed: " + String(speed) + "(km/h)");
      Serial.println("SIV: " + String(SIV));
    }
    Serial.println(F("{DEBUG - LOOP SUCCESS: All Sensor Data Successfully Read! :>}"));
    return true;
}

String dataCompile()
{
  String dataString = "";
  dataString += time;
  dataString += ",";
  dataString += longitude;
  dataString += ",";
  dataString += latitude;
  dataString += ",";
  dataString += altitude;
  dataString += ",";
  dataString += pressure;
  dataString += ",";
  dataString += temperature;
  dataString += ",";
  dataString += roll;
  dataString += ",";
  dataString += pitch;
  dataString += ",";
  dataString += yaw;
  dataString += ",";
  dataString += ax;
  dataString += ",";
  dataString += ay;
  dataString += ",";
  dataString += az;
  dataString += ",";
  dataString += speed;
}
void writeToSD()
{
  datsString = dataCompile(); //String For Logging Data
  Serial.println("{DEBUG - LOOP CHANGE: Data Compiled; Now Writing In SD Card...}");
  File dataFile = SD.open("datalog.csv", FILE_WRITE); //Open File (Only One File Can Open At A Time)
  if(dataFile) { //If File Is Available, Write To It
    msLastWrite = millis();
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString); //Print To Serial Port
    Serial.println("{DEBUG - LOOP SUCCESS: Data Successfully Written! :>}");
  }
  else { //If File Is Not Open, Show Error
    Serial.println("{DEBUG - LOOP ERROR: Error Opening datalog.txt. :<}");
  }
}

void writeToHC12()
{
    digitalWrite(LED_BUILTIN, state); //REVIEW: What Is The Purpose Of This Line?
    String dataString = dataCompile();
    HC12.println(dataString);
    msLastRadio = millis();
    Serial.println("{DEBUG - LOOP SUCCESS: Data Successfully Transmitted To HC12 Radio! :>}");
    state != state;
    /*Recieve From Ground Station:
      if(HC12.available() > 0) {
        Serial.print(millis());
        Serial.print(" RF: ");
        while (HC12.available() > 0) {
          Serial.print(char(HC12.read()));
        }
      }*/
}

void setup() {
  piracyStates STATE = STANDBY;
  availSerial();
  initPinModes();
  initBMP();
  initBNO(); 
  initGNSS();
  initSD();
  initHC12();
  Serial.println(F("{DEBUG - SETUP SUCCESS: All Tests Complete; Control Passed To System Loop!! :D}"));
}

void loop() {
  //Loop-Related Variables:
    //int msLoopBegin = millis();
    //int msLoop = -1;
  //Blink LED:
    //Serial.println("{DEBUG - LOOP CALC: Time Since Last Blink " + String(millis()-msLastBlink) + "ms}");
    if(millis() - msLastBlink >= msBlinkRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastBlink) + "ms >= " + String(msBlinkRate) + "ms.");
      msLastBlink = millis();
      blinkLED();
    }
  //REVIEW: Implement When Sand Deposit Switch Flipped
    //Buzzer Melody:
      if(!buzzerSounded) {
        //Serial.println("{DEBUG - LOOP CALC: Buzzer Notes (" + String(millis() - msLastNote()) + "ms >= " + String(duration * 1.30) + "ms)}");
        if((noteSeq < size) && (millis() - msLastNote >= duration * 1.30)) {
          noTone(BUZZER_PIN); //Stop Playing On The Buzzer
          msLastNote = millis();
          duration = 60 / BPM / (durations[noteSeq]/4); //Duration(sec) Of Given Note
          tone(BUZZER_PIN, notesArr[noteSeq], duration);
          noteSeq++;
        }
        if(noteSeq == size) {
          buzzerSounded = true;
        }
      }

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
      if() { //REVIEW: Signal To Move
        STATE = LAUNCH;
      }
      break;
    case 2: //LAUNCH
      //REVIEW: Sensor Data
      if(altitude > groundLevel) //REVIEW: In Flight; Altitude > Ground Level
        STATE = FLIGHT;
        itFlew = true;
      }
      break;
    case 3: //FLIGHT
      //REVIEW: Sensor Data
      //If Buzzer Has Not Played Freebird, Play It!
      if((altitude >= groundLevel - levelError && altitude <= groundLevel + levelError) && itFlew) { //REVIEW: Finished Flight; Altitude == Ground Level
        STATE = LANDED;
      }
      break;
    case 4: //LANDED
      //REVIEW: Sensor Data
      if(false) { //REVIEW: Stopped Moving
        STATE = CAPTAIN
      }
      break;
    case 5: //CAPTAIN
      //REVIEW: Stop Storing Sensor Data
        //Prepare For Data Analysis:
          //Parse All Data; Check If Realistic (Too Small/Large) and Unrealistic
          //Update Min/Max Values & Create Summary
          //Generate Data Plots As Function of Time
      break;
    default:
      break;
  }

  //Output Sensor Data (REVIEW: Implement Data Analysis):
    //Serial.println("{DEBUG - LOOP CALC: Time Since Last Sensor Data Pull " + String(millis()-msLastPull) + "ms.}");
    if(millis() - msLastPull >= msPullRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastPull) + "ms >= " + String(msPullRate) + "ms; Pulling BMP388 & BNO055 Sensor Data...}");
      if(!readSensors())
        continue;
      msLastPull = millis();
    }   
    if(millis() - msLastGNSSPull >= msGNSSPullRate) {
      Serial.print("{DEBUG - LOOP CHANGE: " + String(millis() - msLastGNSSPull) + "ms >= " + String(msGNSSPullRate) + "ms; Pulling Data...}");
      msLastGNSSPull = millis();
    }
  //SD Card Storage (REVIEW: Implement Beyond Tests):
    if(millis() - msLastWrite >= msWriteRate) { //Review: Can We Open The File Before Compiling Data To Send; It Would Be Optimal?
      Serial.println("{DEBUG - LOOP CHANGE: Preparing Data Transfer To SD Card...}");
      writeToSD();
  }
  //HC12 Radio Transmission (REVIEW: Implement Past Test):
    if(millis() - msLastRadio >= msRadioRate) {
      Serial.println("{DEBUG - LOOP CHANGE: Preparing Data Transfer To HC12 Radio...}");
      writeToHC12();
    }
}
//Printing Values (REVIEW: Thorough Testing):
/*void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //Error-Checking Values
  if (event->type == SENSOR_TYPE_ORIENTATION) {
    Serial.print("Orient:");
    yaw = event->orientation.x; //yaw
    roll = event->orientation.y; //roll
    pitch = event->orientation.z; //pitch
    Serial.print("\tyaw= ");
    Serial.print(x);
    Serial.print(" |\troll= ");
    Serial.print(y);
    Serial.print(" |\tpitch= ");
    Serial.println(z);
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    Serial.print("Linear:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
    ax = event->acceleration.x;
    ay = event->acceleration.y;
    az = event->acceleration.z;
    Serial.print("\tAx= ");
    Serial.print(x);
    Serial.print(" |\tAy= ");
    Serial.print(y);
    Serial.print(" |\tAz= ");
    Serial.println(z);
  }
  Accelerometer (REVIEW: Commented; Do We Plan On Exlcuding?):
    if(event->type == SENSOR_TYPE_ACCELEROMETER) {
      Serial.print("Accl:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    }
  Gyroscope (REVIEW: Commented; Do We Plan On Exlcuding?):
    else if (event->type == SENSOR_TYPE_GYROSCOPE) {
      Serial.print("Gyro:");
      x = event->gyro.x;
      y = event->gyro.y;
      z = event->gyro.z;
    }
  Rotation Vector (REVIEW: Commented; Do We Plan On Exlcuding?):
    else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
      Serial.print("Rot:");
      x = event->gyro.x;
      y = event->gyro.y;
      z = event->gyro.z;
    }
  Linear Acceleration (REVIEW: Commented; Do We Plan On Exlcuding?):
    else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    Serial.print("Mag:");
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
    }
  Gravity (REVIEW: Commented; Do We Plan On Exlcuding?):
    else if (event->type == SENSOR_TYPE_GRAVITY) {
      Serial.print("Gravity:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    }
  //else {
    //Serial.println(F("{DEBUG - PRINT ERROR: Something Went Wrong When Trying To Print Data}"));
    //return;
  }
}*/