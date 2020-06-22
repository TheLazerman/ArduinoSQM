/*
  TSL2591 SQM meter
  TSL2591_SQM Library from Gabe Shaughnessy

  This code by Gabe Coones
  Jan 11 2020
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "SQM_TSL2591.h"

//define display size
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Declaration for TSL2591 Luminosity Sensor
SQM_TSL2591 sqm = SQM_TSL2591(2591);
void readSQM(void);

//variables
int mode = 0;
float average = 0;
float NELM = 0;
int Buzzer = 5; //Piezo buzzer on pin 5
int Button = 7; //Mode select button
unsigned long lastClick = 0; //For button debouncing 

void setup() {
  //start listening on Serial port
  Serial.begin(9600);

  //declar pinmodes
  pinMode(Buzzer, OUTPUT);
  pinMode(Button, INPUT);

  //setup interupt for mode switching
  attachInterrupt(digitalPinToInterrupt(Button),switchMode,RISING);

  //Is our display connected? 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 display failed"));
  }
  
  //display bootup message
  display.clearDisplay();
  display.setCursor(0,0);           
  display.setTextColor(SSD1306_WHITE);        
  display.setTextSize(2);            
  display.print(F("Lazerlabs SQM Meter"));
  display.display();


  //initialize the TSL2591 as our SQM sensor
  if (!sqm.begin()) {
    //Can't find the sensor, complain about it
    display.setCursor(0,0);             
    display.setTextSize(2);            
    display.print(F("TSL2591 Not Found"));
    display.display();
    Serial.println("SQM sensor not found");
    doubleBeep();
  } else{
    //sensor found, initalize
    Serial.println("Found SQM (TSL) sensor");
    sqm.config.gain = TSL2591_GAIN_LOW;
    sqm.config.time = TSL2591_INTEGRATIONTIME_200MS;
    sqm.configSensor();
    sqm.showConfig();
    sqm.setCalibrationOffset(0.0);    
  }

  //everything looks good, beep! 
  singleBeep();
  delay(450);
}

void loop() {
  //stay in your lane. 
  switch (mode){
    case 0:
      mode0();
    break;
    case 1:
      mode1();
    break;   
  } 
  Serial.print("full:   "); Serial.println(sqm.full);
  Serial.print("ir:     "); Serial.println(sqm.ir);
  Serial.print("vis:    "); Serial.println(sqm.vis);
  Serial.print("mpsas:  "); Serial.print(sqm.mpsas);
  Serial.print(" +/- "); Serial.println(sqm.dmpsas);
  Serial.print(F("Lux: ")); Serial.println(sqm.calculateLux(sqm.full, sqm.ir), 6);
  Serial.println("======================================");
}

void mode0(){
  //display Mangatude/arcsecond^2
  sqm.takeReading();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print(sqm.mpsas);
  display.setCursor(60,0);
  display.print(F("mpsas "));  
  display.print(F("+/- "));
  display.print(sqm.dmpsas);
  display.display();
}

void mode1(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print(F("Taking    Average..."));
  display.display(); 
  singleBeep();
  
  for (int i=0; i<5; i++){
    sqm.takeReading();
    average=average + sqm.mpsas;
    delay(1000);
  }
  average = average / 5;
  //calculate NELM (Naked Eye Limiting Magnitude
  NELM = 7.93-5*log(pow(10,4.316-(average/5))+1);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print(average);
  display.setCursor(50,0);
  display.print(F(" mpsas"));
  display.setCursor(0,16);
  display.print(NELM);
  display.print("NELM");
  display.display();  
  doubleBeep();
  delay(10000);
  mode = 0;
}

//Interrupt Serivce Routine
void switchMode(){
  //Debounce
  if(millis() - lastClick < 200){
    return;
  }
  lastClick = millis();
  mode ++; //we are here, so button was pushed. Go to next mode.
  if(mode >= 5){mode = 0;} //we only have 4 modes  
}

//beep twice
void doubleBeep(){  
  digitalWrite(Buzzer, HIGH);
  delay(50);
  digitalWrite(Buzzer,LOW);
  delay(50);
  digitalWrite(Buzzer, HIGH);
  delay(50);
  digitalWrite(Buzzer,LOW);
}

//beep once, comeon it's not that hard
void singleBeep(){
  digitalWrite(Buzzer, HIGH);
  delay(50);
  digitalWrite(Buzzer,LOW);
}
