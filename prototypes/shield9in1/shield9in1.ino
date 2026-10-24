#include <Javino.h>      // https://github.com/chon-group/javino2arduino
#include <DHT.h>         // https://github.com/adafruit/DHT-sensor-library
#include <IRremote.hpp>  // https://github.com/Arduino-IRremote/Arduino-IRremote

#define pinSW1    2
#define pinSW2    3
#define pinDHT    4
#define pinBuzzer 5
#define pinIR     6

#define pinRGB_R  9
#define pinRGB_B  10
#define pinRGB_G  11
#define RedLED    12   
#define BlueLED   13  
#define pinPOT    A0
#define pinLDR    A1
#define pinLM35   A2

Javino javino;
DHT dht(pinDHT, DHT11);
bool buzzerStatus = false;
uint16_t lastIRCommand = 0;
bool irReceived = false;


void setup() {
  confDefaultPinOut();
  confJavinoCMDs();
}

void confJavinoCMDs(){
  
  /* Javino Acts and Percept description */
  javino.act["ledBlue(on)"]  = ledBlueOn;
  javino.act["ledBlue(off)"] = ledBlueOff;
  javino.act["ledRed(on)"]   = ledRedOn;
  javino.act["ledRed(off)"]  = ledRedOff;
  
  javino.act["rgb(red)"]     = rgbRed;
  javino.act["rgb(green)"]   = rgbGreen;
  javino.act["rgb(blue)"]    = rgbBlue;
  javino.act["rgb(white)"]   = rgbWhite;
  javino.act["rgb(off)"]     = rgbOff;

  javino.act["buzzer(on)"]   = buzzerOn;
  javino.act["buzzer(off)"]  = buzzerOff;
  javino.perceive(getExogenousPerceptions);
  javino.start(9600);
}

void confDefaultPinOut(){
  pinMode(pinSW1,INPUT);
  pinMode(pinSW2,INPUT);
  pinMode(BlueLED,OUTPUT);
  pinMode(RedLED,OUTPUT);
  pinMode(pinBuzzer,OUTPUT);
  pinMode(pinRGB_R,OUTPUT);
  pinMode(pinRGB_G,OUTPUT);
  pinMode(pinRGB_B,OUTPUT);

  setRGB(0,0,0);
  dht.begin();
  IrReceiver.begin(pinIR, DISABLE_LED_FEEDBACK);
}


void loop() {
  readIR();
  javino.run();
}


void serialEvent(){javino.readSerial();}

void getExogenousPerceptions(){
  //char msg[24];

  if(digitalRead(BlueLED)){javino.addPercept("blue(on)");}
  else {javino.addPercept("blue(off)");}


  if(digitalRead(RedLED)){javino.addPercept("red(on)");}
  else {javino.addPercept("red(off)");}

 
  bool r = digitalRead(pinRGB_R);
  bool g = digitalRead(pinRGB_G);
  bool b = digitalRead(pinRGB_B);
  if(r && g && b){javino.addPercept("rgb(white)");}
  else if(r && !g && !b){javino.addPercept("rgb(red)");}
  else if(!r && g && !b){javino.addPercept("rgb(green)");}
  else if(!r && !g && b){javino.addPercept("rgb(blue)");}
  else if(!r && !g && !b){javino.addPercept("rgb(off)");}

  
  if(buzzerStatus){javino.addPercept("buzzer(on)");}
  else {javino.addPercept("buzzer(off)");}

  
  if(digitalRead(pinSW1)==LOW){javino.addPercept("button1(pressed)");}
  else {javino.addPercept("button1(released)");}
  
  if(digitalRead(pinSW2)==LOW){javino.addPercept("button2(pressed)");}
  else {javino.addPercept("button2(released)");}

  /*
  snprintf_P(msg,sizeof(msg),PSTR("luminosity(%d)"),lerAnalogico(pinLDR));
  javino.addPercept(msg);

  
  snprintf_P(msg,sizeof(msg),PSTR("potentiometer(%d)"),lerAnalogico(pinPOT));
  javino.addPercept(msg);


  /* DHT11 sensor: on failure, only dht(error) is sent */
//  float temp = dht.readTemperature();
//  float humi = dht.readHumidity();
//  if(isnan(temp) || isnan(humi)){
//    javino.addPercept(String(F("dht(error)")));
//  }else {
//    snprintf_P(msg,sizeof(msg),PSTR("temperature(%d)"),(int)round(temp));
//    javino.addPercept(msg);
//    snprintf_P(msg,sizeof(msg),PSTR("humidity(%d)"),(int)round(humi));
//    javino.addPercept(msg);
//  }


  

  javino.addPercept(getIRPercepts());
  javino.addPercept(getTemperaturePercept());
  javino.addPercept(getHumidityPercept());

}


String getTemperaturePercept() {
    float temp = dht.readTemperature();

    if (isnan(temp)) {return "temp(error)";}

    int temp10 = (int)round(temp * 10);

    return "temp(" +
        String(temp10 / 10) + "." +
        String(abs(temp10 % 10)) + ")";
}

String getHumidityPercept() {
    float humi = dht.readHumidity();

    if (isnan(humi)) {return "humidity(error)";}

    int humi10 = (int)round(humi * 10);

    return "humidity(" +
        String(humi10 / 10) + "." +
        String(abs(humi10 % 10)) + ")";
}

String getIRPercepts() {
    if (irReceived) {
        irReceived = false;
        return "cmd(" + String(lastIRCommand) + ")";
    }
    return "cmd(none)";
}

/* It implements the commands to be executed in the exogenous environment. 
*
* NOTE: Every command must reflect in a function. 
*
*/
void ledBlueOn(){digitalWrite(BlueLED,HIGH);}
void ledBlueOff(){digitalWrite(BlueLED,LOW);}
void ledRedOn(){digitalWrite(RedLED,HIGH);}
void ledRedOff(){digitalWrite(RedLED,LOW);}
void buzzerOn(){tone(pinBuzzer,1000); buzzerStatus=true;}
void buzzerOff(){noTone(pinBuzzer); buzzerStatus=false;}
void rgbRed()     { setRGB(255,   0,   0); }
void rgbGreen()   { setRGB(  0, 255,   0); }
void rgbBlue()    { setRGB(  0,   0, 255); }
void rgbWhite()   { setRGB(255, 255, 255); }
void rgbOff()     { setRGB(0, 0, 0); }


int lerAnalogico(uint8_t pino) {
  analogRead(pino);
  delayMicroseconds(200);
  return analogRead(pino);
}

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(pinRGB_R, r);
  analogWrite(pinRGB_G, g);
  analogWrite(pinRGB_B, b);
}



void readIR() {
    if (IrReceiver.decode()) {
        if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
            lastIRCommand = IrReceiver.decodedIRData.command;
            irReceived = true;
        }

        IrReceiver.resume();
    }
}
