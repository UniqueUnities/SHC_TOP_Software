//program created with the help of programs from https://github.com/daar/HC-12

//for uart
#define rxPin 0
#define txPin 1
#define HC12 Serial1

long baud = 9600;
bool state = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  // define pin modes for tx, rx:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(baud);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial monitor available... OK");

  Serial.print("Serial link available... ");
  HC12.begin(9600);
  Serial.println("initialization done.");
}

void loop() {
  // put your main code here, to run repeatedly:
  //send to ground station
  digitalWrite(LED_BUILTIN, state);

  /*dataString += time;
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

  dataString += Ax;
  dataString += ",";

  dataString += Ay;
  dataString += ",";

  dataString += Az;
  dataString += ",";

  dataString += speed;*/

  HC12.println("test123");
  Serial.println("sent: test123");
  delay(1000);

  state != state;

  //recieve from ground station
  /*if (HC12.available() > 0) {

    Serial.print(millis());
    Serial.print(" RF: ");

    while (HC12.available() > 0) {
      
      Serial.print(char(HC12.read()));
    }
  }*/

  delay(100);
}
