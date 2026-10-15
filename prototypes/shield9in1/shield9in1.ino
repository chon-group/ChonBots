#include <Javino.h>   /* https://javino.chon.group */
#include <DHT.h>

#define pinSW1    2
#define pinSW2    3
#define pinDHT    4
#define pinBuzzer 5
#define pinRGB_R  9
#define pinRGB_B  10
#define pinRGB_G  11
#define pinLED2   12   /* red  */
#define pinLED1   13   /* blue */
#define pinPOT    A0
#define pinLDR    A1
#define pinLM35   A2

Javino javino;
DHT dht(pinDHT, DHT11);
bool buzzerStatus = false;

void setup() {
  pinMode(pinSW1,INPUT);
  pinMode(pinSW2,INPUT);
  pinMode(pinLED1,OUTPUT);
  pinMode(pinLED2,OUTPUT);
  pinMode(pinBuzzer,OUTPUT);
  pinMode(pinRGB_R,OUTPUT);
  pinMode(pinRGB_G,OUTPUT);
  pinMode(pinRGB_B,OUTPUT);
  dht.begin();
  setRGB(0,0,0);

  /* Javino Acts and Percept description */
  javino.act[String(F("led1On"))]    = led1On;
  javino.act[String(F("led1Off"))]   = led1Off;
  javino.act[String(F("led2On"))]    = led2On;
  javino.act[String(F("led2Off"))]   = led2Off;
  javino.act[String(F("rgbRed"))]    = rgbRed;
  javino.act[String(F("rgbGreen"))]  = rgbGreen;
  javino.act[String(F("rgbBlue"))]   = rgbBlue;
  javino.act[String(F("rgbWhite"))]  = rgbWhite;
  javino.act[String(F("rgbOff"))]    = rgbOff;
  javino.act[String(F("buzzerOn"))]  = buzzerOn;
  javino.act[String(F("buzzerOff"))] = buzzerOff;
  javino.perceive(getExogenousPerceptions);
  javino.start(9600);
}

void loop() {javino.run();}

/*
 * The serialEvent() function handles interruptions coming from the serial port.
 * 
 * NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or other ATmega32U4 based boards. 
 * https://docs.arduino.cc/built-in-examples/communication/SerialEvent 
 * 
 */
void serialEvent(){javino.readSerial();}

/* 
  It sends the exogenous environment's perceptions to the agent. 
*/
void getExogenousPerceptions(){
  char msg[24];

  /* LED1 (blue) */
  if(digitalRead(pinLED1)){
    javino.addPercept(String(F("led1(on)")));
  }else {
    javino.addPercept(String(F("led1(off)")));
  }

  /* LED2 (red) */
  if(digitalRead(pinLED2)){
    javino.addPercept(String(F("led2(on)")));
  }else {
    javino.addPercept(String(F("led2(off)")));
  }

  /* RGB LED */
  bool r = digitalRead(pinRGB_R);
  bool g = digitalRead(pinRGB_G);
  bool b = digitalRead(pinRGB_B);
  if(r && g && b){
    javino.addPercept(String(F("rgb(white)")));
  }else if(r && !g && !b){
    javino.addPercept(String(F("rgb(red)")));
  }else if(!r && g && !b){
    javino.addPercept(String(F("rgb(green)")));
  }else if(!r && !g && b){
    javino.addPercept(String(F("rgb(blue)")));
  }else if(!r && !g && !b){
    javino.addPercept(String(F("rgb(off)")));
  }

  /* Buzzer */
  if(buzzerStatus){
    javino.addPercept(String(F("buzzer(on)")));
  }else {
    javino.addPercept(String(F("buzzer(off)")));
  }

  /* Buttons (active LOW) */
  if(digitalRead(pinSW1)==LOW){
    javino.addPercept(String(F("button1(pressed)")));
  }else {
    javino.addPercept(String(F("button1(released)")));
  }
  if(digitalRead(pinSW2)==LOW){
    javino.addPercept(String(F("button2(pressed)")));
  }else {
    javino.addPercept(String(F("button2(released)")));
  }

  /* LDR sensor (0-1023) */
  snprintf_P(msg,sizeof(msg),PSTR("luminosity(%d)"),lerAnalogico(pinLDR));
  javino.addPercept(msg);

  /* Potentiometer (0-1023) */
  snprintf_P(msg,sizeof(msg),PSTR("potentiometer(%d)"),lerAnalogico(pinPOT));
  javino.addPercept(msg);

  /* LM35 sensor: 10 mV per Celsius degree, so each mV is 0.1 C */
  long tenths = lerAnalogico(pinLM35) * 5000L / 1023;
  snprintf_P(msg,sizeof(msg),PSTR("lm35(%d.%d)"),(int)(tenths/10),(int)(tenths%10));
  javino.addPercept(msg);

  /* DHT11 sensor: on failure, only dht(error) is sent */
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();
  if(isnan(temp) || isnan(humi)){
    javino.addPercept(String(F("dht(error)")));
  }else {
    snprintf_P(msg,sizeof(msg),PSTR("temperature(%d)"),(int)round(temp));
    javino.addPercept(msg);
    snprintf_P(msg,sizeof(msg),PSTR("humidity(%d)"),(int)round(humi));
    javino.addPercept(msg);
  }
}

/* It implements the commands to be executed in the exogenous environment. 
*
* NOTE: Every command must reflect in a function. 
*
*/
void led1On(){digitalWrite(pinLED1,HIGH);}
void led1Off(){digitalWrite(pinLED1,LOW);}
void led2On(){digitalWrite(pinLED2,HIGH);}
void led2Off(){digitalWrite(pinLED2,LOW);}
void rgbRed(){setRGB(255,0,0);}
void rgbGreen(){setRGB(0,255,0);}
void rgbBlue(){setRGB(0,0,255);}
void rgbWhite(){setRGB(255,255,255);}
void rgbOff(){setRGB(0,0,0);}
void buzzerOn(){tone(pinBuzzer,1000); buzzerStatus=true;}
void buzzerOff(){noTone(pinBuzzer); buzzerStatus=false;}

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
