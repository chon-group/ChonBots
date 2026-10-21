
/*
 * Multi-purpose Shield V1
 *
 * https://www.usinainfo.com.br/shieldsexpansores/shield-multifuncoes-hy-m302-para-arduino-com-dht11-lm35-receptor-ir-ldr-leds-buzzer-e-outros-4866.html
 * https://wiki.keyestudio.com/Ks0183_keyestudio_Multi-purpose_Shield_V1
 */

#include <Javino.h>      // https://github.com/chon-group/javino2arduino
#include <DHT.h>         // https://github.com/adafruit/DHT-sensor-library and https://github.com/adafruit/adafruit_sensor
#include <IRremote.hpp>  // https://github.com/Arduino-IRremote/Arduino-IRremote

// ==================================================
// PIN CONFIGURATION
// ==================================================

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

// ==================================================
// GLOBAL VARIABLES
// ==================================================

Javino javino;
DHT dht(pinDHT, DHT11);

// Actuator states
uint8_t rgbStatus = 0;

// IR event
uint16_t lastIRCommand = 0;
bool irReceived = false;

// Button events
bool button1Pressed = false;
bool button2Pressed = false;

// Button debounce
bool lastSW1 = HIGH;
bool lastSW2 = HIGH;

bool stableSW1 = HIGH;
bool stableSW2 = HIGH;

unsigned long lastDebounceSW1 = 0;
unsigned long lastDebounceSW2 = 0;

const unsigned long DEBOUNCE_DELAY = 30;

// DHT11 cached readings (x10)
int temperature10 = 0;
int humidity10 = 0;

bool dhtValid = false;

unsigned long lastDHTRead = 0;
const unsigned long DHT_INTERVAL = 2000;

// Shared buffer for numeric perceptions
char msg[24];

bool buzzerStatus = false;
bool buzzerLevel = false;

unsigned long lastBuzzerToggle = 0;

const unsigned long BUZZER_HALF_PERIOD = 500;

// ==================================================
// SETUP
// ==================================================

void setup() {
    confDefaultPinOut();
    confJavinoCMDs();
}

void confDefaultPinOut() {

    pinMode(pinSW1, INPUT);
    pinMode(pinSW2, INPUT);

    pinMode(BlueLED, OUTPUT);
    pinMode(RedLED, OUTPUT);
    pinMode(pinBuzzer, OUTPUT);

    pinMode(pinRGB_R, OUTPUT);
    pinMode(pinRGB_G, OUTPUT);
    pinMode(pinRGB_B, OUTPUT);

    setRGB(0, 0, 0);

    dht.begin();
    IrReceiver.begin(pinIR, DISABLE_LED_FEEDBACK);

    // Initialize button states
    lastSW1 = stableSW1 = digitalRead(pinSW1);
    lastSW2 = stableSW2 = digitalRead(pinSW2);
}

void confJavinoCMDs() {

    // LEDs
    javino.act["ledBlueOn"]  = ledBlueOn;
    javino.act["ledBlueOff"] = ledBlueOff;
    javino.act["ledRedOn"]   = ledRedOn;
    javino.act["ledRedOff"]  = ledRedOff;

    // RGB
    javino.act["rgbRed"]   = rgbRed;
    javino.act["rgbGreen"] = rgbGreen;
    javino.act["rgbBlue"]  = rgbBlue;
    javino.act["rgbWhite"] = rgbWhite;
    javino.act["rgbOff"]   = rgbOff;

    // Buzzer
    javino.act["buzzerOn"]  = buzzerOn;
    javino.act["buzzerOff"] = buzzerOff;

    // Perceptions
    javino.perceive(getExogenousPerceptions);
    javino.start(9600);
}

// ==================================================
// MAIN LOOP
// ==================================================

void loop() {
    javino.run();
    readIR();
    readButtons();
    readDHT();
    updateBuzzer();
    
}

void serialEvent() {
    javino.readSerial();
}

// ==================================================
// EXOGENOUS PERCEPTIONS
// ==================================================

void getExogenousPerceptions() {

    // LEDs
    javino.addPercept(
        digitalRead(BlueLED) ?
        F("blue(on)") : F("blue(off)")
    );

    javino.addPercept(
        digitalRead(RedLED) ?
        F("red(on)") : F("red(off)")
    );

    // RGB
    switch (rgbStatus) {
        case 1:
            javino.addPercept(F("rgb(red)"));
            break;
        case 2:
            javino.addPercept(F("rgb(green)"));
            break;
        case 3:
            javino.addPercept(F("rgb(blue)"));
            break;
        case 4:
            javino.addPercept(F("rgb(white)"));
            break;
        default:
            javino.addPercept(F("rgb(off)"));
    }

    // Buzzer
    javino.addPercept(
        buzzerStatus ?
        F("buzzer(on)") :
        F("buzzer(off)")
    );

    // Buttons
    getButtonPercepts();

    // Analog sensors
    snprintf_P(msg, sizeof(msg),
        PSTR("lum(%d)"),
        lerAnalogico(pinLDR));
    javino.addPercept(msg);

    snprintf_P(msg, sizeof(msg),
        PSTR("pot(%d)"),
        lerAnalogico(pinPOT));
    javino.addPercept(msg);

    // IR
    getIRPercept();

    // DHT11
    getDHTPercepts();
}

// ==================================================
// BUTTONS - EVENT CAPTURE
// ==================================================

void readButtons() {

    bool sw1 = digitalRead(pinSW1);
    bool sw2 = digitalRead(pinSW2);

    unsigned long now = millis();

    // Button 1
    if (sw1 != lastSW1) {
        lastDebounceSW1 = now;
    }

    if ((now - lastDebounceSW1) >= DEBOUNCE_DELAY) {
        if (sw1 != stableSW1) {
            stableSW1 = sw1;

            if (stableSW1 == LOW) {
                button1Pressed = true;
            }
        }
    }

    lastSW1 = sw1;

    // Button 2
    if (sw2 != lastSW2) {
        lastDebounceSW2 = now;
    }

    if ((now - lastDebounceSW2) >= DEBOUNCE_DELAY) {
        if (sw2 != stableSW2) {
            stableSW2 = sw2;

            if (stableSW2 == LOW) {
                button2Pressed = true;
            }
        }
    }

    lastSW2 = sw2;
}

// ==================================================
// BUTTONS - PERCEPTIONS
// ==================================================

void getButtonPercepts() {

    if (button1Pressed) {
        javino.addPercept(F("button1(pressed)"));
        button1Pressed = false;
    } else {
        javino.addPercept(F("button1(released)"));
    }

    if (button2Pressed) {
        javino.addPercept(F("button2(pressed)"));
        button2Pressed = false;
    } else {
        javino.addPercept(F("button2(released)"));
    }
}

// ==================================================
// IR - EVENT CAPTURE
// ==================================================

void readIR() {

    if (IrReceiver.decode()) {

        if (!(IrReceiver.decodedIRData.flags &
              IRDATA_FLAGS_IS_REPEAT)) {

            lastIRCommand =
                IrReceiver.decodedIRData.command;

            irReceived = true;
        }

        IrReceiver.resume();
    }
}

// ==================================================
// IR - PERCEPTION
// ==================================================

void getIRPercept() {

    if (irReceived) {

        snprintf_P(msg, sizeof(msg),
            PSTR("cmd(%u)"),
            lastIRCommand);

        javino.addPercept(msg);
        irReceived = false;

    } else {
        javino.addPercept(F("cmd(none)"));
    }
}

// ==================================================
// DHT11 - SENSOR READING
// ==================================================

void readDHT() {

    unsigned long now = millis();

    if (now - lastDHTRead < DHT_INTERVAL) {
        return;
    }

    lastDHTRead = now;

    float temp = dht.readTemperature();
    float humi = dht.readHumidity();

    // Preserve previous values on failure
    if (isnan(temp) || isnan(humi)) {
        return;
    }

    // Update only when both readings are valid
    temperature10 = (int)round(temp * 10);
    humidity10 = (int)round(humi * 10);

    dhtValid = true;
}

// ==================================================
// DHT11 - PERCEPTIONS
// ==================================================

void getDHTPercepts() {

    if (!dhtValid) {
        javino.addPercept(F("dht(error)"));
        return;
    }

    snprintf_P(msg, sizeof(msg),
        PSTR("temp(%d.%d)"),
        temperature10 / 10,
        abs(temperature10 % 10));

    javino.addPercept(msg);

    snprintf_P(msg, sizeof(msg),
        PSTR("hum(%d.%d)"),
        humidity10 / 10,
        abs(humidity10 % 10));

    javino.addPercept(msg);
}

// ==================================================
// LED ACTIONS
// ==================================================

void ledBlueOn() {digitalWrite(BlueLED, HIGH);}
void ledBlueOff() {digitalWrite(BlueLED, LOW);}
void ledRedOn() {digitalWrite(RedLED, HIGH);}
void ledRedOff() {digitalWrite(RedLED, LOW);}

// ==================================================
// BUZZER ACTIONS
// ==================================================

void buzzerOn() {
    buzzerStatus = true;
    buzzerLevel = false;
    lastBuzzerToggle = micros();
    digitalWrite(pinBuzzer, LOW);
}

void buzzerOff() {
    buzzerStatus = false;
    buzzerLevel = false;
    digitalWrite(pinBuzzer, LOW);
}

void updateBuzzer() {
    if (!buzzerStatus) {
        return;
    }

    unsigned long now = micros();

    if (now - lastBuzzerToggle >= BUZZER_HALF_PERIOD) {
        lastBuzzerToggle = now;
        buzzerLevel = !buzzerLevel;
        digitalWrite(pinBuzzer, buzzerLevel);
    }
}

// ==================================================
// RGB ACTIONS
// ==================================================

void rgbRed() {setRGB(255, 0, 0);rgbStatus = 1;}
void rgbGreen() {setRGB(0, 255, 0);rgbStatus = 2;}
void rgbBlue() {setRGB(0, 0, 255);rgbStatus = 3;}
void rgbWhite() {setRGB(255, 255, 255);rgbStatus = 4;}
void rgbOff() {setRGB(0, 0, 0);rgbStatus = 0;}

// ==================================================
// UTILITIES
// ==================================================

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
