#include <Javino.h>         //https://github.com/chon-group/javino2arduino
#include <LiquidCrystal.h>  //https://www.arduino.cc/reference/en/libraries/liquidcrystal/

// When using a physical arduino boar with the LCD Keypad Shield, uncomment the following:
/*
  #define PinLCDCtrl01 4
  #define PinLCDCtrl02 5
  #define PinLCDCtrl03 6
  #define PinLCDCtrl04 7
  #define PinLCDRS     8
  #define PinLCDEN     9
  #define PinLCDLight  10
  #define LedRed       11
  #define LedYellow    12
  #define LedGreen     13
*/

// When using a simulated arduino with the SimulIDE, uncomment the following:
/*
  #define PinLCDCtrl01 5
  #define PinLCDCtrl02 4
  #define PinLCDCtrl03 3
  #define PinLCDCtrl04 2
  #define PinLCDRS     11
  #define PinLCDEN     10
  #define PinLCDLight  12
  #define LedRed       7
  #define LedYellow    8
  #define LedGreen     9
*/


Javino javino;
LiquidCrystal lcd(PinLCDRS, PinLCDEN, PinLCDCtrl01, PinLCDCtrl02, PinLCDCtrl03, PinLCDCtrl04);
unsigned long lastClick = millis();
String strBtnPressed = "";
String lastLCDMsg = "";
int option = 0;
int temperature=25;
int airHumidity=50;
int rainLast24hrs=0;

void serialEvent(){
  javino.readSerial();
}

void setup() {
 pinMode(LedRed,OUTPUT);
 pinMode(LedGreen,OUTPUT);
 pinMode(LedYellow,OUTPUT);
 startLCDWithKeyboard();
 javino.start(9600);
}
 
void loop() {
 readLCDKeyboard();
 if(javino.availableMsg()){
  if(javino.requestPercepts()) getExogenousPerceptions();
  else if(javino.requestAction("redAlert")) led("red");
  else if(javino.requestAction("yellowAlert")) led("yellow");
  else if(javino.requestAction("greenAlert")) led("green");
  else if(javino.requestAction("alertOff")) led("off");
  else information(javino.getMsg());
 }
}

void information(String info){
    lastLCDMsg = "";
    lcd.clear();
    writeInLCD(info);
}

void getExogenousPerceptions(){
    javino.addPercept("device(arduinoWithLCDKeypadShield)");
    javino.addPercept("humidity("+String(airHumidity)+")");
    javino.addPercept("temperature("+String(temperature)+")");
    javino.addPercept("rainLast24hrs("+String(rainLast24hrs)+")");
    javino.sendPercepts();
}

void startLCDWithKeyboard(){
  pinMode(PinLCDLight, OUTPUT);
  digitalWrite(PinLCDLight, HIGH);
  lcd.begin(16, 2);
  printScreen();
 }

void readLCDKeyboard(){ 
  strBtnPressed = getBtnPressed();
  if((strBtnPressed != "none" ) & (millis()>(lastClick+500))){

      lastClick = millis();

      if(strBtnPressed == "select") option=0;
      else if(strBtnPressed == "down") option++;
      else if(strBtnPressed == "up") option--;
      printScreen();
      
  }
  delay(50);
}


void printScreen(){
  if(option<=0){
    writeInLCD("Smart Home ...");
    writeInLCD(" ");
    option=0;
  }else if(option==1){
    if(strBtnPressed=="right") temperature++;
    if(strBtnPressed=="left")  temperature--;
    writeInLCD("Temperature...");
    writeInLCD(String(temperature)+"c");       
  }else if(option==2){
    if(strBtnPressed=="right") airHumidity++;
    if(strBtnPressed=="left")  airHumidity--;
    writeInLCD("Air Humidity");
    writeInLCD(String(airHumidity)+"%");      
  }else if(option>=3){
    if(strBtnPressed=="right") rainLast24hrs = rainLast24hrs+5;
    if(strBtnPressed=="left")  rainLast24hrs = rainLast24hrs-5;
    writeInLCD("Rain last 24hrs");
    writeInLCD(String(rainLast24hrs)+"mm");  
    option=3;    
  }
}


String getBtnPressed(){
    int input = analogRead(A0);
    if     (input < 50) { return "right";}
    else if(input < 150){ return "up";}
    else if(input < 350){ return "down";}
    else if(input < 550){ return "left";}
    else if(input < 750){ return "select";}
    else return "none"; 
}

void writeInLCD(String strL){
  if (strL.startsWith("\"") && strL.endsWith("\"")) {
    strL.remove(0, 1);
    strL.remove(strL.length() - 1); 
  }

  int lengthStr = strL.length();
  for (int startIndex = 0; startIndex < lengthStr; startIndex += 16) {
    int endIndex = startIndex + 15;
    if (endIndex >= lengthStr) {
      endIndex = lengthStr - 1;
    }
    String chunk = strL.substring(startIndex, endIndex + 1);
    digitalWrite(PinLCDLight, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(lastLCDMsg);
    lcd.setCursor(0,1);
    lcd.print(chunk);
    lastLCDMsg = chunk;
  }
}

void led(String op){
  if(op=="off"){
    digitalWrite(LedGreen,LOW);
    digitalWrite(LedRed,LOW);
    digitalWrite(LedYellow,LOW);
  }else if(op=="green"){
    led("off");
    digitalWrite(LedGreen,HIGH);
  }else if(op=="yellow"){
    led("off");
    digitalWrite(LedYellow,HIGH);
  }else if(op=="red"){
    led("off");
    digitalWrite(LedRed,HIGH);
  }
}
