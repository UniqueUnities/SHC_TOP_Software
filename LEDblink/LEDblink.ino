#define LED_WHITE 22

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_WHITE,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long lastTime = 0;
  bool lightOn = false;
  if(millis() - lastTime >= 500 && lightOn == false)
  {
    digitalWrite(LED_WHITE,HIGH);
    lightOn = true;
    lastTime = millis();
  }
  if(millis() - lastTime >= 500 && lightOn == true)
  {
    digitalWrite(LED_WHITE,LOW);
    lightOn = false;
    lastTime = millis();
  }
}
