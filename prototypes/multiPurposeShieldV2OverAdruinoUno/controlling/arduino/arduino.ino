#include <Wire.h>
#include <MultiFuncShield.h>  // https://github.com/hpsaturn/MultiFuncShield-Library
#include <Javino.h> //https://github.com/chon-group/javino2arduino
#include <string.h>
#include <stdio.h>

Javino javino;

// Display positions 1..4, from left to right.
char displayText[5] = "0000";
bool displayDirty = true;

// Button presses are retained until the next perception request.
bool btn1Pressed = false;
bool btn2Pressed = false;
bool btn3Pressed = false;

void serialEvent() {
    javino.readSerial();
}

bool parseNumber(const String &msg, const char *prefix, int &value) {
    if (!msg.startsWith(prefix) || !msg.endsWith(")")) return false;
    String s = msg.substring(strlen(prefix), msg.length() - 1);
    if (s.length() == 0) return false;
    for (unsigned int i = 0; i < s.length(); i++) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    value = s.toInt();
    return true;
}

void setDigit(byte position, char character) {
    if (position < 1 || position > 4) return;
    displayText[position - 1] = character;
    displayDirty = true;
}

void setTwoDigits(byte start, int value) {
    setDigit(start,     '0' + value / 10);
    setDigit(start + 1, '0' + value % 10);
}

void updateDisplay() {
    if (!displayDirty) return;
    MFS.write(displayText);
    displayDirty = false;
}

void setLed(byte index, bool enabled) {
    switch (index) {
        case 1: MFS.writeLeds(LED_1, enabled ? ON : OFF); break;
        case 2: MFS.writeLeds(LED_2, enabled ? ON : OFF); break;
        case 3: MFS.writeLeds(LED_3, enabled ? ON : OFF); break;
        case 4: MFS.writeLeds(LED_4, enabled ? ON : OFF); break;
    }
}

void readButtons() {
    switch (MFS.getButton()) {
        case BUTTON_1_PRESSED: btn1Pressed = true; break;
        case BUTTON_2_PRESSED: btn2Pressed = true; break;
        case BUTTON_3_PRESSED: btn3Pressed = true; break;
    }
}

void sendPerceptions() {
    javino.addPercept(F("device(multiFunctionShieldV2)"));
    javino.addPercept(btn1Pressed ? F("btn1(pressed)") : F("btn1(released)"));
    javino.addPercept(btn2Pressed ? F("btn2(pressed)") : F("btn2(released)"));
    javino.addPercept(btn3Pressed ? F("btn3(pressed)") : F("btn3(released)"));
    javino.sendPercepts();
    btn1Pressed = btn2Pressed = btn3Pressed = false;
}

void handleCommand(const String &msg) {
    if (msg == "alarmOn") {
        MFS.beep(10, 5, 4, 100, 50);
        return;
    }
    if (msg == "alarmOff") {
        MFS.beep(0);
        return;
    }

    int value;
    if (parseNumber(msg, "setHour(", value) && value <= 23) {
        setTwoDigits(1, value);
        return;
    }
    if (parseNumber(msg, "setMin(", value) && value <= 59) {
        setTwoDigits(3, value);
        return;
    }

    // setDisplay1(0)..setDisplay4(9), one character per position.
    if (msg.startsWith("setDisplay") && msg.length() == 14 &&
        msg[10] >= '1' && msg[10] <= '4' &&
        msg[11] == '(' && msg[13] == ')' &&
        msg[12] >= '0' && msg[12] <= '9') {
        setDigit(msg[10] - '0', msg[12]);
        return;
    }

    // led1On / led1Off ... led4On / led4Off
    if (msg.startsWith("led") && msg.length() >= 6 &&
        msg[3] >= '1' && msg[3] <= '4') {
        byte index = msg[3] - '0';
        if (msg.substring(4) == "On") setLed(index, true);
        else if (msg.substring(4) == "Off") setLed(index, false);
    }
}

void setup() {
    MFS.initialize();
    for (byte i = 1; i <= 4; i++) setLed(i, false);
    MFS.beep(0);
    updateDisplay();
    javino.start(9600);
}

void loop() {
    readButtons();
    updateDisplay();

    if (!javino.availableMsg()) return;
    if (javino.requestPercepts()) {
        sendPerceptions();
    } else {
        String msg = javino.getMsg();
        msg.trim();
        handleCommand(msg);
    }
}
