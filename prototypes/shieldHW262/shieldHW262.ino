#include <Javino.h>   /* https://javino.chon.group */

#define pinLatch   4
#define pinClock   7
#define pinData    8
#define pinBuzzer  3
#define pinPOT     A0
#define BUZZER_ON  LOW
#define BUZZER_OFF HIGH

#define MODO_OFF     0
#define MODO_POT     1
#define MODO_COUNTER 2
#define MODO_ALL     3

const uint8_t LEDS[4]   = {13, 12, 11, 10};
const uint8_t BOTOES[3] = {A1, A2, A3};
const byte SEG[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
const byte SEL[4]  = {0xF1, 0xF2, 0xF4, 0xF8};

Javino javino;
uint8_t modoDisplay = MODO_POT;
int valor = 0;

void setup() {
  pinMode(pinLatch,OUTPUT);
  pinMode(pinClock,OUTPUT);
  pinMode(pinData,OUTPUT);
  pinMode(pinBuzzer,OUTPUT);
  digitalWrite(pinBuzzer,BUZZER_OFF);
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(LEDS[i],OUTPUT);
    digitalWrite(LEDS[i],HIGH);          /* HIGH = apagado */
  }
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(BOTOES[i],INPUT_PULLUP);
  }

  javino.act[String(F("led1On"))]         = led1On;
  javino.act[String(F("led1Off"))]        = led1Off;
  javino.act[String(F("led2On"))]         = led2On;
  javino.act[String(F("led2Off"))]        = led2Off;
  javino.act[String(F("led3On"))]         = led3On;
  javino.act[String(F("led3Off"))]        = led3Off;
  javino.act[String(F("led4On"))]         = led4On;
  javino.act[String(F("led4Off"))]        = led4Off;
  javino.act[String(F("buzzerOn"))]       = buzzerOn;
  javino.act[String(F("buzzerOff"))]      = buzzerOff;
  javino.act[String(F("displayPot"))]     = displayPot;
  javino.act[String(F("displayCounter"))] = displayCounter;
  javino.act[String(F("displayAll"))]     = displayAll;
  javino.act[String(F("displayOff"))]     = displayOff;
  javino.perceive(getExogenousPerceptions);
  javino.start(9600);
}

void loop() {
  javino.run();
  atualizarValor();
  atualizarDisplay();
}

void serialEvent(){javino.readSerial();}

/*
  It sends the exogenous environment's perceptions to the agent.
*/
void getExogenousPerceptions(){
  char msg[24];

  for (uint8_t i = 0; i < 4; i++) {
    if (digitalRead(LEDS[i]) == LOW) {
      snprintf_P(msg,sizeof(msg),PSTR("led%d(on)"),i+1);
    } else {
      snprintf_P(msg,sizeof(msg),PSTR("led%d(off)"),i+1);
    }
    javino.addPercept(msg);
  }

  if (digitalRead(pinBuzzer) == BUZZER_ON) {
    strcpy_P(msg,PSTR("buzzer(on)"));
  } else {
    strcpy_P(msg,PSTR("buzzer(off)"));
  }
  javino.addPercept(msg);

  for (uint8_t i = 0; i < 3; i++) {
    if (digitalRead(BOTOES[i]) == LOW) {
      snprintf_P(msg,sizeof(msg),PSTR("button%d(pressed)"),i+1);
    } else {
      snprintf_P(msg,sizeof(msg),PSTR("button%d(released)"),i+1);
    }
    javino.addPercept(msg);
  }

  snprintf_P(msg,sizeof(msg),PSTR("potentiometer(%d)"),lerAnalogico(pinPOT));
  javino.addPercept(msg);

  /* Display mode */
  switch (modoDisplay) {
    case MODO_POT:     strcpy_P(msg,PSTR("display(pot)"));     break;
    case MODO_COUNTER: strcpy_P(msg,PSTR("display(counter)")); break;
    case MODO_ALL:     strcpy_P(msg,PSTR("display(all)"));     break;
    default:           strcpy_P(msg,PSTR("display(off)"));     break;
  }
  javino.addPercept(msg);
}

/* It implements the commands to be executed in the exogenous environment.
*
* NOTE: Every command must reflect in a function.
*
*/
void led1On(){setLed(0,true);}
void led1Off(){setLed(0,false);}
void led2On(){setLed(1,true);}
void led2Off(){setLed(1,false);}
void led3On(){setLed(2,true);}
void led3Off(){setLed(2,false);}
void led4On(){setLed(3,true);}
void led4Off(){setLed(3,false);}
void buzzerOn(){digitalWrite(pinBuzzer,BUZZER_ON);}
void buzzerOff(){digitalWrite(pinBuzzer,BUZZER_OFF);}
void displayPot(){modoDisplay = MODO_POT;}
void displayCounter(){modoDisplay = MODO_COUNTER;}
void displayAll(){modoDisplay = MODO_ALL;}
void displayOff(){modoDisplay = MODO_OFF;}

void setLed(uint8_t i, bool ligado) {
  digitalWrite(LEDS[i], ligado ? LOW : HIGH);
}

int lerAnalogico(uint8_t pino) {
  analogRead(pino);
  delayMicroseconds(200);
  return analogRead(pino);
}

/* Atualiza o numero mostrado. O pot e lido a cada 100 ms para nao atrasar a multiplexacao. */
void atualizarValor() {
  static unsigned long ultimaLeitura = 0;
  if (modoDisplay == MODO_COUNTER) {
    valor = (millis() / 1000) % 10000;
  } else if (modoDisplay == MODO_POT && millis() - ultimaLeitura >= 100) {
    ultimaLeitura = millis();
    valor = lerAnalogico(pinPOT);
  }
}

/* Mostra um digito por chamada; as 4 posicoes completam um ciclo a cada 4 chamadas. */
void atualizarDisplay() {
  static uint8_t pos = 0;
  static const int limite[4] = {1000, 100, 10, 0};
  int digitos[4] = {valor / 1000 % 10, valor / 100 % 10, valor / 10 % 10, valor % 10};
  byte seg = SEG[digitos[pos]];
  if (modoDisplay == MODO_OFF) seg = 0xFF;            /* tudo apagado */
  else if (modoDisplay == MODO_ALL) seg = 0x00;       /* todos os segmentos acesos */
  else if (valor < limite[pos]) seg = 0xFF;           /* zero a esquerda apagado */

  digitalWrite(pinLatch, LOW);
  shiftOut(pinData, pinClock, MSBFIRST, seg);
  shiftOut(pinData, pinClock, MSBFIRST, SEL[pos]);
  digitalWrite(pinLatch, HIGH);
  pos = (pos + 1) % 4;
}
