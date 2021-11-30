/* Copyright (C) zuhail pm - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by zuhail pm on April 2021
 * https://github.com/zuhl-c
 */
 
#include <Wire.h>
#include <PT2323.h> 
#include <PT2322.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <IRremote.h>

//IR Hex codes
#define IR_RE  0xFFFFFFFF//ir_repeat
#define POW    0x807FBA45//power
#define IN_SW  0x807F3AC5//input
#define MUTE_S 0x807FFA05//mute
#define _3D_SW 0x807F7A85//3d
#define VOL_P  0x807F32CD//vol ++
#define VOL_M  0x807F52AD//vol --
#define FR_P   0x807F08F7//front ++
#define FR_M   0x807F8877//front --
#define RR_P   0x807F18E7//rear++
#define RR_M   0x807F9867//rear--
#define CT_P   0x807F28D7//center++
#define CT_M   0x807FA857//center--
#define SUB_P  0x807F30CF//sub++
#define SUB_M  0x807FB04F//sub--
#define BAS_P  0x807F9A65//bass++
#define BAS_M  0x807FDA25//bass--
#define MID_P  0x807F1AE5//mid++
#define MID_M  0x807FAA55//mid--
#define TR_P   0x807F5AA5//treble++
#define TR_M   0x807F2AD5//treble--

//Rotary encoder pins
#define DAT 10
#define CLK 11
#define BTN 12


   PT2323 pt23; 
   PT2322 pt22;

   int IR_PIN=9;//ir sensor input//
   int backlight=8;//lcd backlight
   int menu,mute,in,rom,_3d,new_menu,cur_menu;
   int volume,bass,mid,treble,front,rear,center,sub;
   unsigned long time,key_value=0;
   boolean disable,light;


   LiquidCrystal lcd(2, 3, 4, 5, 6, 7);// RS,E,D4,D5,D6,D7 connection LCD
   IRrecv irrecv(IR_PIN);
   decode_results ir;

   String MENUS[]={"AUDIO-SYSTEM","INPUT","VOLUME","FRONT","SURROUND",
                   "CENTER","SUB OUT","BASS","MIDDLE","TREBLE",
                   "3D SOUND","MUTE","ON ","OFF"};

void setup() {

  pinMode(backlight,OUTPUT);
  pinMode(CLK,INPUT);
  pinMode(DAT,INPUT);
  pinMode(BTN,INPUT_PULLUP);
  digitalWrite(backlight,HIGH);
    
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(16,2);
  irrecv.enableIRIn();
  
 printDev();readRom();audio_23();audio_22();
  
}

void loop() {

  
   //managing ir code//
  if(irrecv.decode(&ir)){
    
    if(ir.value==IR_RE){
      ir.value=key_value;
    }
    switch (ir.value){
     //input function//
      case IN_SW:menu=1;break;
     //volume //
     case  VOL_P: case VOL_M:menu=2;break;
     //front//
     case FR_P: case FR_M:menu=3;break;
     //rear//
     case RR_P: case RR_M:menu=4;break;
     //center//
     case CT_P: case CT_M:menu=5;break;
     //subwoofer
     case SUB_P: case SUB_M:menu=6;break;
     //bass
     case BAS_P: case BAS_M:menu=7;break;
     //mid
     case MID_P: case MID_M:menu=8;break;
     //treble
     case TR_P: case TR_M:menu=9;break;
     //3d function//
     case _3D_SW:menu=10;break;
     //mute function//
     case MUTE_S:menu=11;break;
     }
    key_value=ir.value;
  delay(100);
  irrecv.resume();
  Serial.println(ir.value,HEX);
  }

//Rotary encoder functions
  if(digitalRead(BTN)==LOW){if(millis()-time>50){ menu++;cl();if(menu>10){menu=1;}rom=1;} time=millis();}

//    currentState=digitalRead(CLK);
//    
//    if(currentState!=lastState&&currentState==1){
//      if(digitalRead(DAT)!=digitalRead(CLK)){
//     Serial.print("DATA PIN : ");Serial.println(digitalRead(DAT));
//     Serial.print("CLOCK PIN : ");Serial.println(digitalRead(CLK));
//        Serial.println("CLOCK WISE >>>");
//      }
//      if(digitalRead(DAT)==digitalRead(CLK)){
//     Serial.print("DATA PIN : ");Serial.println(digitalRead(DAT));
//     Serial.print("CLOCK PIN : ");Serial.println(digitalRead(CLK));
//        Serial.println("ANNTI CLOCK WISE <<<");
//      }
//    }
//  lastState=currentState;

//Remote controll functions    
   //input//
    if(menu==1)
   {
   if(ir.value==IN_SW){cl();in++;if(in>4){in=0;}audio_23();rom=1;time=millis();}
   printMenu(in+1);
  }      
    //volume//
    if(menu==2)
  {
    //remote controlled
     if(ir.value==VOL_P){volume++;if(volume>=79){volume=79;}cl();audio_22();rom=1;time=millis();}
     if(ir.value==VOL_M){volume--;if(volume<=0){volume=0;}cl();audio_22();rom=1;time=millis();}
     printMenu(volume);
   }
   if(menu==3){
   //front//
    if(ir.value==FR_P){front++;if(front>15){front=15;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==FR_M){front--;if(front<0){front=0;}cl();audio_22();rom=1;time=millis();}
    printMenu(front);
    }
    if(menu==4){
  //surround//
    if(ir.value==RR_P){rear++;if(rear>15){rear=15;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==RR_M){rear--;if(rear<0){rear=0;}cl();audio_22();rom=1;time=millis();}
    printMenu(rear);
    }
   if(menu==5){
  //center//
    if(ir.value==CT_P){center++;if(center>15){center=15;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==CT_M){center--;if(center<0){center=0;}cl();audio_22();rom=1;time=millis();}
    printMenu(center);
    }
    if(menu==6){
  //subwoofer//
    if(ir.value==SUB_P){sub++;if(sub>15){sub=15;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==SUB_M){sub--;if(sub<0){sub=0;}cl();audio_22();rom=1;time=millis();}
    printMenu(sub);
    }
    //bass//
    if(menu==7)
   {
    if(ir.value==BAS_P){bass++;if(bass>7){bass=7;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==BAS_M){bass--;if(bass<-7){bass=-7;}cl();audio_22();rom=1;time=millis();}
    printMenu(bass*2);
   }
   //mid//
   if(menu==8)
   {
    if(ir.value==MID_P){mid++;if(mid>7){mid=7;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==MID_M){mid--;if(mid<-7){mid=-7;}cl();audio_22();rom=1;time=millis();}
    printMenu(mid*2);
   }
   //treble//
    if(menu==9)
   {
    if(ir.value==TR_P){treble++;if(treble>7){treble=7;}cl();audio_22();rom=1;time=millis();}
    if(ir.value==TR_M){treble--;if(treble<-7){treble=-7;}cl();audio_22();rom=1;time=millis();}
    printMenu(treble*2);
   }
   //3d//
   if(menu==10)
   {
    if(_3d>1){_3d=0;}
    if(_3d==0&&ir.value==_3D_SW){_3d=1;audio_22();cl();rom=1;time=millis();}
    if(_3d==1&&ir.value==_3D_SW){_3d=0;audio_22();cl();rom=1;time=millis();}
    printMenu_2(_3d);
   }
   if(menu==11)
   {
    if(ir.value==MUTE_S&&mute==0){cl();mute=1;audio_22();audio_23();}
    if(ir.value==MUTE_S&&mute==1){cl();mute=0;audio_22();audio_23();}
    printMenu_2(mute);
   }
   cur_menu=menu;
   
    //eeprom saving//
 if(millis()-time>5000&&rom==1){
  updateRom();cl();rom=0;
  if(mute==1){menu=11;cl();}
  else{menu=0;printDev();}
 }
  
}

void audio_23(){
   pt23.setInput(in);//Serial.print("input : ");Serial.println(in);
   pt23.setFeature(0,0);//enchanced surround function enabled
   pt23.setMute(0,0,0,0,0,0,mute);//6 channels enabling//
}
void audio_22(){
  //activating inputs
   pt22.ActiveIn();
   pt22.setVolume(abs(volume));//Serial.print("volume : ");Serial.println(volume);
  
   //6channel 
   pt22.setFrontLeft(front); //Serial.print("front : ");Serial.println(front);
   pt22.setFrontRight(front);
   pt22.setSurroundLeft(rear); //Serial.print("rear : ");Serial.println(rear);
   pt22.setSurroundRight(rear);
   pt22.setCenter(center);// Serial.print("center : ");Serial.println(center);
   pt22.setSubwoofer(sub); //Serial.print("sub : ");Serial.println(sub);

   //tune-control,mute,_3d//
   pt22.setBass(bass);
   pt22.setMiddle(mid);
   pt22.setTreble(treble);
   pt22.setFunc(mute,_3d,0);
   //Serial.print("bass : ");Serial.println(bass);
   //Serial.print("mid : ");Serial.println(mid);
   //Serial.print("treble : ");Serial.println(treble);
   //Serial.print("mute : ");Serial.println(mute);
   //Serial.print("3d : ");Serial.println(_3d);

}

void cl(){
  if(menu==cur_menu){ir.value=0;}
  else{ir.value=0;lcd.clear();}
}
void updateRom(){
     EEPROM.update(0,volume);
     EEPROM.update(1,bass);
     EEPROM.update(2,mid);
     EEPROM.update(3,treble);
     EEPROM.update(4,front);
     EEPROM.update(5,rear);
     EEPROM.update(6,center);
     EEPROM.update(7,sub);
     EEPROM.update(8,_3d);
     EEPROM.update(9,in);
}
void readRom(){
  volume=EEPROM.read(0);
  bass=EEPROM.read(1);
  mid=EEPROM.read(2);
  treble=EEPROM.read(3);
  front=EEPROM.read(4);
  rear=EEPROM.read(5);
  center=EEPROM.read(6);
  sub=EEPROM.read(7);
  _3d=EEPROM.read(8);
  in=EEPROM.read(9);
}
void printMenu(int val){

  if(MENUS[menu].length()<7){
    lcd.setCursor(1,0);
    lcd.print(MENUS[menu]);
  }
  else{
    lcd.setCursor(0,0);
    lcd.print(MENUS[menu]);
  }
  printDigit(val);
}

void printMenu_2(int a){

  byte cx[2][8]={ {B00000,B00001,B00011,B01111,B01111,B01111,B00011,B00001},
                 {B00000,B00000,B01010,B00100,B01010,B00000,B00000,B00000}
               };
  
  lcd.createChar(0,cx[0]);
  lcd.createChar(1,cx[1]);

    if(menu==10){
    lcd.setCursor(4,0);lcd.print(MENUS[menu]);lcd.setCursor(7,1);
    switch(a){
      case 0:lcd.print(MENUS[12]);break;
      case 1:lcd.print(MENUS[13]);break;
    }
  }
  if(menu==11){
    switch(a){
      case 1:lcd.setCursor(6,0);lcd.print(MENUS[menu]);
             lcd.setCursor(7,1);lcd.write(byte(0));lcd.write(byte(1));delay(500);
             lcd.setCursor(7,1);lcd.print("  ");delay(500);break;
      case 0:delay(200);lcd.clear();menu=2;rom=1;time=millis();break;
    }
  }

}

void printDev(){

  lcd.clear();
  byte zuhl[4][8]={ 
    { B11111,B10001,B11101,B10001,B10111,B10001,B11111,B00000},//z
    { B11111,B10101,B10101,B10101,B10101,B10001,B11111,B00000},//u
    { B11111,B10101,B10101,B10001,B10101,B10101,B11111,B00000},//h
    { B11111,B10111,B10111,B10111,B10111,B10001,B11111,B00000},//l
 };
 
  lcd.createChar(0,zuhl[0]);
  lcd.createChar(1,zuhl[1]);
  lcd.createChar(2,zuhl[2]);
  lcd.createChar(3,zuhl[3]);
  
  lcd.setCursor(6,0);lcd.write(byte(0));lcd.write(byte(1));
  lcd.write(byte(2));lcd.write(byte(3));
  lcd.setCursor(2,1);lcd.print(MENUS[0]);
}

void printDigit(int val){
  
int col=9;

byte DIG[8][8] = {
  {B00111,B01111,B11111,B11111,B11111,B11111,B11111,B11111},
  { B11111,B11111,B11111,B00000,B00000,B00000,B00000,B00000},
  { B11100,B11110,B11111,B11111,B11111,B11111,B11111,B11111},
  { B11111,B11111,B11111,B11111,B11111,B11111,B01111,B00111},
  { B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111},
  { B11111,B11111,B11111,B11111,B11111,B11111,B11110,B11100},
  { B11111,B11111,B11111,B00000,B00000,B00000,B11111,B11111},
  { B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111}
};

  lcd.createChar(0,DIG[0]);
  lcd.createChar(1,DIG[1]);
  lcd.createChar(2,DIG[2]);
  lcd.createChar(3,DIG[3]);
  lcd.createChar(4,DIG[4]);
  lcd.createChar(5,DIG[5]);
  lcd.createChar(6,DIG[6]);
  lcd.createChar(7,DIG[7]);

  if(val<0){val=abs(val);lcd.setCursor(7,1);lcd.print("-");}
  else{lcd.setCursor(7,1);lcd.print(" ");}
  
  printDigits(val/10,col);
  printDigits(val%10,col+4);
}
void printDigits(int digits, int x){
  switch (digits) {
  case 0: digit_0(x);break;
  case 1: digit_1(x);break;
  case 2: digit_2(x);break;
  case 3: digit_3(x);break;
  case 4: digit_4(x);break;
  case 5: digit_5(x);break;
  case 6: digit_6(x);break;
  case 7: digit_7(x);break;
  case 8: digit_8(x);break;
  case 9: digit_9(x);break;
  }
}
void digit_0(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0); 
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3); 
  lcd.write(4); 
  lcd.write(5);
}
void digit_1(int x){
  lcd.setCursor(x,0);
  lcd.write(1);
  lcd.write(2);
  lcd.print(" ");
  lcd.setCursor(x,1);
  lcd.write(4);
  lcd.write(7);
  lcd.write(4);
}
void digit_2(int x){
  lcd.setCursor(x,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}
void digit_3(int x){
  lcd.setCursor(x,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}
void digit_4(int x){
  lcd.setCursor(x,0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(7);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}
void digit_5(int x){
  lcd.setCursor(x,0);
  lcd.write(3);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}
void digit_6(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}
void digit_7(int x){
  lcd.setCursor(x,0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}
void digit_8(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}
void digit_9(int x){
  lcd.setCursor(x,0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}
