//===============================================
// SPOT WELDER CONTROL
//
// By: Digital Jester
// Version: 1.0
// 
//===============================================
// Inlude Libaries
//===============================================
#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>

//===============================================
// Define Pins
//===============================================
#define clk 3
#define dt 4
#define sw 5

#define trigger 2
#define pulse 6
#define tonepin 7

#define fan 8

#define tempA A0
#define tempB A1

//===============================================
// Define Icons
//===============================================
#define icoEBat 64
#define icoBell 65
#define icoGear 66
#define icoDArrow 67
#define icoHome 68
#define icoPen 69
#define icoWave 70
#define icoESign 71
#define icoSpan 72
#define icoFBat 73
#define icoBT 74
#define icoCon 75
#define icoUK1 76
#define icoUK2 77
#define icoPower 78
#define icoBack 79
#define icoWifi 80
//===============================================
// Define Fonts
//===============================================
#define smlFont u8g2_font_timB10_tr
#define bigFont u8g2_font_fub25_tn
#define smlIcon u8g2_font_open_iconic_embedded_2x_t
#define bigIcon u8g2_font_open_iconic_embedded_4x_t

//===============================================
// Declare Varibles
//===============================================
volatile boolean TurnDetected = false;
volatile boolean up = false;
volatile boolean button = false;
volatile boolean weld = false;
volatile boolean playtone = false;
volatile boolean menuopt = false;
unsigned long db=0;
unsigned long tdb=0;

int sp = 750;
int p1 = 200;
int p2 = 450;
int dpd = 250;

byte inc = 10;
byte screen = 0;
byte screenRem = 0;
byte menupos = 0;
byte count=0;
byte weldDelay = 5;

float temp0 = 0;
float temp1 = 0;

//===============================================
// Setup Screen Object
//===============================================
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//===============================================
// Define Functions
//===============================================
void triggered() {
    weld = true;
}
void turn()  {
  TurnDetected = true;
  up = (digitalRead(clk) == digitalRead(dt));
}
void singlePulse(){
  digitalWrite(pulse,HIGH);
  delay(sp);
  digitalWrite(pulse,LOW);
}
void doublePulse(){
  digitalWrite(pulse,HIGH);
  delay(p1);
  digitalWrite(pulse,LOW);
  delay(dpd);
  digitalWrite(pulse,HIGH);
  delay(p2);
  digitalWrite(pulse,LOW);
}
int chkMax(int chk){
  if(chk>1000){chk=1000;}
  return chk;
}
int chkMin(int chk){
  if(chk<50){chk=50;}
  return chk;
}
float chkTemp(int pin){
  float vol = (analogRead(pin)/1024.0) * 5;
  float tempC = (vol - .5) * 100;
  return tempC;
}

void setup() {
  // put your setup code here, to run once:
  
  // Set Pin Modes
  pinMode(sw, INPUT_PULLUP);
  pinMode(clk, INPUT);
  pinMode(dt, INPUT);
  pinMode(trigger, INPUT_PULLUP);
  pinMode(tempA, INPUT);
  pinMode(tempB, INPUT);
  pinMode(pulse,OUTPUT);
  digitalWrite(pulse,LOW);
  pinMode(tonepin,OUTPUT);
  pinMode(fan,OUTPUT);
  digitalWrite(fan,LOW);
  
  // Add Interruts
  attachInterrupt(digitalPinToInterrupt(clk), turn, FALLING);
  attachInterrupt(digitalPinToInterrupt(trigger), triggered, FALLING);
  
  u8g2.begin();  // Start LCD Display
  db=millis();  // Set Debonce
  tdb=millis(); // Set Debonce
  
  draw();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
  
//===============================================
// WELDING
//===============================================
  if(weld){
    
    switch (screen){
      case 1:
        if(playtone){tone(tonepin,2093,250);}
        screenRem=screen;
        screen=99;
        count=0;
        if(!weldDelay==0){
        while(count<weldDelay+1){
          draw();
          if(count<weldDelay){delay(1000);}
          count++;
        }
        }else{
          draw();
        }
        singlePulse();
        screen=screenRem;
        draw();
      break;

      case 2:
        if(playtone){tone(tonepin,2093,250);}
        screenRem=screen;
        screen=99;
        count=0;
        if(!weldDelay==0){
        while(count<weldDelay+1){
          draw();
          if(count<weldDelay){delay(1000);}
          count++;
        }
        }else{
          draw();
        }
        doublePulse();
        screen=screenRem;
        draw();
      break;

      default:

      break;
    }
    weld=false;
    draw();
  }

//===============================================
// USER INPUT - TURN
//===============================================
  if (TurnDetected) {
    switch (screen){
      case 0:
        if(up){
          menupos++;
          if (menupos>6){menupos=0;}
        }else{
          menupos--;
          if (menupos>6){menupos=6;}
        }
      break;

      case 1:
          if(up){
            sp=sp+inc;
            sp=chkMax(sp);
          }else{
            sp=sp-inc;
            sp=chkMin(sp);
          }
      break;

      case 2:
      if(menuopt){
        if(menupos==0){
          if(up){
            p1=p1+inc;
            p1=chkMax(p1);
          }else{
            p1=p1-inc;
            p1=chkMin(p1);
          }
      }
      if(menupos==1){
        if(up){
          dpd=dpd+inc;
          if(dpd>1000){dpd=1000;}
        }else{
          dpd=dpd-inc;
          if(dpd<10){dpd=10;}
        }
      }
      if(menupos==2){
        if(up){
          p2=p2+inc;
          p2=chkMax(p2);
        }else{
          p2=p2-inc;
          p2=chkMin(p2);
        }
      }
      }else{
        if(up){
          menupos++;
          if (menupos>3){menupos=0;}
        }else{
          menupos--;
          if (menupos>3){menupos=3;}
        }
      }
      break;

      case 3:
      if(menuopt){
        if(menupos==0){
          if(up){
            inc++;
          }else{
            inc--;
          }
      }
      if(menupos==1){
        if(up){
          weldDelay++;
        }else{
          weldDelay--;
        }
      }
      if(menupos==2){
        if(playtone){
          playtone=false;
        }else{
          playtone=true;
        }
      }
      }else{
        if(up){
          menupos++;
          if (menupos>3){menupos=0;}
        }else{
          menupos--;
          if (menupos>3){menupos=3;}
        }
      }
      break;
    }
    
    draw();
    TurnDetected=false;
    
  }
  
//===============================================
// TEMP - Debounce
//===============================================
 if(millis()-tdb>=1000){
  
  tdb=millis(); // Reset Debounce
  
  temp0=chkTemp(tempA);
  temp1=chkTemp(tempB);
  
  if (temp0<30 && temp1<30){
    digitalWrite(fan, LOW);
  } else {
    digitalWrite(fan, HIGH);
  }
  
  if (screen==90) {draw();}
 }
 
//===============================================
// SWITCH - Debunce
//===============================================
 if(millis()-db>=250){
  if (digitalRead(sw) == LOW) {
    button = true;
    db=millis();  // Reset Debounce
  }
 }
 //===============================================
 // USER INPUT - BUTON PRESS
 //===============================================
  if(button){

    switch (screen){
      case 0:
        if(menupos==0){screen=1;menupos=0;}
        if(menupos==1){screen=2;menupos=0;}
        if(menupos==2){screen=3;menupos=0;}

        if(menupos==6){screen=90;menupos=0;}
      break;

      case 1:
        screen=0;
        menupos=0;
      break;

      case 2:        
        if(menupos==3){
          screen=0;
          menupos=0;
        }else{
          if(menuopt){
            menuopt=false;
          }else{
            menuopt=true;
          }
        }
      break;

      case 3:        
        if(menupos==3){
          screen=0;
          menupos=0;
        }else{
          if(menuopt){
            menuopt=false;
          }else{
            menuopt=true;
          }
        }
      break;

      case 90:
        screen=0;
        menupos=0;
      break;
    }

    draw();
    button=false;
    
  }
}

void draw(){
  switch (screen){
    case 0:
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.setCursor(40,13); // Set Cursor
        u8g2.print("MENU");
        u8g2.drawHLine(2,18,124);
      if(menupos<=2){
        u8g2.setCursor(2,31); // Set Cursor
        if(menupos==0){u8g2.print(">");}
        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("Single Pulse");
        
        u8g2.setCursor(2,44); // Set Cursor
        if(menupos==1){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        u8g2.print("Double Pulse");

        u8g2.setCursor(2,57); // Set Cursor
        if(menupos==2){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        u8g2.print("Settings");
      }
      if((menupos>2)&&(menupos<=5)){
        u8g2.setCursor(2,31); // Set Cursor
        if(menupos==3){u8g2.print(">");}
        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("Save");
        
        u8g2.setCursor(2,44); // Set Cursor
        if(menupos==4){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        u8g2.print("Load");

        u8g2.setCursor(2,57); // Set Cursor
        if(menupos==5){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        u8g2.print("About");
      }
      if((menupos>5)&&(menupos<=8)){
        u8g2.setCursor(2,31); // Set Cursor
        if(menupos==6){u8g2.print(">");}
        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("Temp");
        
        u8g2.setCursor(2,44); // Set Cursor
        if(menupos==7){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        //u8g2.print("Load");

        u8g2.setCursor(2,57); // Set Cursor
        if(menupos==8){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        //u8g2.print("About");
      }
        drawIcon(0,16,icoHome);
      } while ( u8g2.nextPage() ); // End Screen

      
    break;

    case 1:
    
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        //u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
        u8g2.setCursor(20,15); // Set Cursor
        u8g2.print("SINGLE PULSE");
        u8g2.drawHLine(2,20,124);
        u8g2.setFont(bigFont);
        u8g2.setCursor(52,55);
        if(sp>9){u8g2.setCursor(39,55);} // Set Cursor
        if(sp>99){u8g2.setCursor(27,55);} // Set Cursor
        if(sp>999){u8g2.setCursor(14,55);} // Set Cursor
        u8g2.print(sp);
        //u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
        u8g2.setFont(smlFont); // Set Font
        u8g2.print(" ms");
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoWave);
      } while ( u8g2.nextPage() ); // End Screen

    
    break;

    case 2:
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        //u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
        u8g2.setCursor(20,13); // Set Cursor
        u8g2.print("DOUBLE PULSE");
        u8g2.drawHLine(2,18,124);
        //u8g2.setFont(bigFont);

        u8g2.setCursor(2,31); // Set Cursor
        if((menupos==0)&&(menuopt==false)){u8g2.print(">");}
        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("1st: ");
        u8g2.setCursor(60,31); // Set Cursor
        if((menupos==0)&&(menuopt==true)){u8g2.print("[");}
        u8g2.print(p1);
        u8g2.print(" ms");
        if((menupos==0)&&(menuopt==true)){u8g2.print("]");}

        u8g2.setCursor(2,44); // Set Cursor
        if((menupos==1)&&(menuopt==false)){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        u8g2.print("Delay: ");
        u8g2.setCursor(60,44); // Set Cursor
        if((menupos==1)&&(menuopt==true)){u8g2.print("[");}
        u8g2.print(dpd);
        u8g2.print(" ms");
        if((menupos==1)&&(menuopt==true)){u8g2.print("]");}

        u8g2.setCursor(2,57); // Set Cursor
        if((menupos==2)&&(menuopt==false)){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        u8g2.print("2nd: ");
        u8g2.setCursor(60,57); // Set Cursor
        if((menupos==2)&&(menuopt==true)){u8g2.print("[");}
        u8g2.print(p2);
        u8g2.print(" ms");
        if((menupos==2)&&(menuopt==true)){u8g2.print("]");}

        if(menupos==3){drawIcon(112,64,icoBack);}
        
        drawIcon(0,16,icoWave);
      } while ( u8g2.nextPage() ); // End Screen
    break;

    case 3:
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        //u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
        u8g2.setCursor(30,13); // Set Cursor
        u8g2.print("SETTINGS");
        u8g2.drawHLine(2,18,124);
        //u8g2.setFont(bigFont);

        u8g2.setCursor(2,31); // Set Cursor
        if((menupos==0)&&(menuopt==false)){u8g2.print(">");}
        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("Incrament: ");
        if((menupos==0)&&(menuopt==true)){u8g2.print("[");}
        u8g2.print(inc);
        if((menupos==0)&&(menuopt==true)){u8g2.print("]");}

        u8g2.setCursor(2,44); // Set Cursor
        if((menupos==1)&&(menuopt==false)){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        u8g2.print("Weld Delay: ");
        if((menupos==1)&&(menuopt==true)){u8g2.print("[");}
        u8g2.print(weldDelay);
        if((menupos==1)&&(menuopt==true)){u8g2.print("]");}

        u8g2.setCursor(2,57); // Set Cursor
        if((menupos==2)&&(menuopt==false)){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        u8g2.print("Buzzer: ");
        if((menupos==2)&&(menuopt==true)){u8g2.print("[");}
        if(playtone){
          u8g2.print("ON");
        }else{
          u8g2.print("OFF");
        }
        //u8g2.print(playtone);
        if((menupos==2)&&(menuopt==true)){u8g2.print("]");}

        
        if(menupos==3){drawIcon(112,64,icoBack);}
        
        drawIcon(0,16,icoSpan);
      } while ( u8g2.nextPage() ); // End Screen
    break;

    case 90:
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(30,13,"TEMP");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(2,31); // Set Cursor
        u8g2.print("Sensor 0: ");
        u8g2.print(temp0);
        u8g2.setCursor(2,44); // Set Cursor
        u8g2.print("Sensor 1: ");
        u8g2.print(temp1);
        u8g2.setCursor(2,57); // Set Cursor
        u8g2.print("Fan: ");
        if (digitalRead(fan) == LOW) {
          u8g2.print("OFF");
        } else {
          u8g2.print("ON");
        }
        
        drawIcon(0,16,icoESign);
        drawIcon(112,16,icoESign);
        
      } while ( u8g2.nextPage() ); // End Screen
    break;
    
    case 99:
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(30,13,"WELDNG");
        u8g2.drawHLine(2,18,124);
        
        drawIcon(0,16,icoDArrow);
        drawIcon(112,16,icoDArrow);
        
        if(count==weldDelay){
          u8g2.setFont(bigIcon); // Icons
          u8g2.drawGlyph(15, 55, icoDArrow); 
          u8g2.drawGlyph(81, 55, icoDArrow); 
        }else{
          u8g2.setFont(bigFont);
          u8g2.setCursor(57,55);
          if(weldDelay-count>9){u8g2.setCursor(41,55);} // Set Cursor
          if(weldDelay-count>99){u8g2.setCursor(32,55);} // Set Cursor
          u8g2.print(weldDelay-count);
        
          u8g2.setFont(bigIcon); // Icons
          u8g2.drawGlyph(0, 55, icoESign); 
          u8g2.drawGlyph(96, 55, icoESign); 
        }
      } while ( u8g2.nextPage() ); // End Screen
    break;
  }
  
}
void drawIcon(int x, int y, int icon){

  // Draw Icon Based On Selected
  u8g2.setFont(smlIcon); // Icons
  u8g2.drawGlyph(x, y, icon);      
}
