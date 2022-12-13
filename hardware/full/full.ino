#include <Servo.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 10; //mcu > HX711 dout pin
const int HX711_sck = 11; //mcu > HX711 sck pin
//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;
Servo myservo1;
Servo myservo2;
int lcdColumns = 16;
int lcdRows = 2;
const int green1 = 2;
const int red1 = 3;
const int green2 = 4;
const int red2 = 5;
const int buzzer = 7;
int pos = 0;
int weight1=100;
int weight2=100;
int y=0;
void setup() {
  myservo1.attach(6);
  myservo2.attach(9);
  pinMode(green1, OUTPUT);
  pinMode(red1, OUTPUT);
  pinMode(green2, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  //digitalWrite(green1, HIGH);
  //digitalWrite(green2, HIGH);
  digitalWrite(buzzer, LOW);
  delay(3000);
  myservo1.write(0);
  myservo2.write(0);
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() {
    //outertakeo();
    //delay(3000);
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0; //increase value to slow down serial print activity
  
    // check for new data/start next conversion:
    if (LoadCell.update()) newDataReady = true;
  
    // get smoothed value from the dataset:
    if (newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float i = LoadCell.getData();
        Serial.print("Load_cell output val: ");
        Serial.println(i);
        newDataReady = 0;
        t = millis();
        
        if(i>50&&y==0){
          intakeo();
          y=1;
          }
          if(i<50&&y==1){
          intakec();
          y=0;
          }
      }
    }
 
    // receive command from serial terminal, send 't' to initiate tare operation:
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 't') LoadCell.tareNoDelay();
    }
  
    // check if last tare operation is complete:
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
    }
  }
void intakeo(){
  digitalWrite(red1, HIGH);
  digitalWrite(green1, LOW);
  digitalWrite(buzzer, HIGH);
  delay(1000);
  for (pos = 0; pos <= 100; pos += 1) {
    myservo1.write(pos);
    delay(15);
  }
}
void intakec(){
  digitalWrite(red1, LOW);
  digitalWrite(green1, HIGH);
  digitalWrite(buzzer, LOW);
  delay(1000);
  for (pos = 100; pos >= 0; pos -= 1) {
    myservo1.write(pos);
    delay(15);
  }
}
void outertakeo(){
  digitalWrite(red2, HIGH);
  digitalWrite(green2, LOW);
  digitalWrite(buzzer, HIGH);
  delay(1000);
  for (pos = 100; pos >= 0; pos -= 1) {
    myservo2.write(pos);
    delay(15);
  }
  }
