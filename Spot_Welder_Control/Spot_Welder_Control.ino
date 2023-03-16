//===================================================================================
//  SPOT WELDER CONTROL
//  -------------------
//
//  Copyright (c) 2023 iDIY Down Under
//  Author: Julian Carmichael (aka Digital Jester)
// 
//  Software Version: 0.1 beta 
//  For PCB Revision 1.0
//
//  GitHub: https://github.com/idiydownunder/arduino-spot-welder-control
// 
//  This work is licensed under a Creative Commons 
//  Attribution-NonCommercial-ShareAlike 4.0 International License
//  http://creativecommons.org/licenses/by-nc-sa/4.0/
//
//===================================================================================
#define ver "0.1 beta"


//===================================================================================
// Inlude Libaries
//===================================================================================
#include <Wire.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <U8g2lib.h>

//===================================================================================
// Define Pins
//===================================================================================
//Rotary Encoder Pins
#define clk 3
#define dt 4
#define sw 5

#define trigger 2 // Trigger Welding Switch
#define pulse 6   // Welding Pulse Pin
#define tonepin 7 // Buzzer Pin

#define fan 8     // Cooling Fan I/O Pin 

#define tempA A0  // Temp Sen. 0 Pin
#define tempB A1  // Temp Sen. 1 Pin

//===================================================================================
// Define Default Values and Limits
//===================================================================================

#define def_sp 750  // Single Pulse Default Time
#define min_sp 10   // Single Pulse Min Time
#define max_sp 2500 // Single Pulse Max Time

#define def_p1 150  // Double Pulse Default Time (First Pulse)
#define def_p2 550  // Double Pulse Default Time (Second Pulse)
#define min_p 10    // Double Pulse Min Time
#define max_p 1000  // Double Pulse Max Time

#define def_dpd 100 // Double Pulse Delay Default Time
#define min_dpd 10  // Double Pulse Delay Min Time
#define max_dpd 2000// Double Pulse Delay Max Time

#define def_inc 10  // Pulse Incrament Default Value
#define min_inc 1   // Pulse Incrament Min Value
#define max_inc 100 // Pulse Incrament Max Value

#define def_weldDelay 5   // Weld Delay Timer Default Time
#define min_weldDelay 0   // Weld Delay Timer Min Time
#define max_weldDelay 10  // Weld Delay Timer Max Time

#define def_playtone true // Default Buzzer Status

//===================================================================================
// Define Fonts
//===================================================================================
#define smlFont u8g2_font_timB10_tr
#define bigFont u8g2_font_fub25_tn
#define smlIcon u8g2_font_open_iconic_embedded_2x_t
#define bigIcon u8g2_font_open_iconic_embedded_4x_t

//===================================================================================
// Define Icons
//===================================================================================
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

//===================================================================================
// Declare Global Run Time Varibles
//===================================================================================
volatile boolean TurnDetected = false;
volatile boolean up = false;
volatile boolean button = false;
volatile boolean weld = false;
volatile boolean playtone = def_playtone;
volatile boolean menuopt = false;
unsigned long db=0;
unsigned long tdb=0;

int sp = def_sp;

int p1 = def_p1;
int dpd = def_dpd;
int p2 = def_p2;

byte inc = def_inc;
byte weldDelay = def_weldDelay;

byte screen = 0;
byte screenRem = 0;
byte menupos = 0;
byte count=0;

float temp0 = 0;
float temp1 = 0;

//===================================================================================
// Setup Screen Object
//===================================================================================
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//===================================================================================
// Global Functions
//===================================================================================
void triggered() {
  // User Ready To Weld
    weld = true;  // Set Flag
}
void turn()  {
  // User Input Detected
  TurnDetected = true;
  up = (digitalRead(clk) == digitalRead(dt));
}
void singlePulse(){
  // Single Pusle Welding
  digitalWrite(pulse,HIGH);
  delay(sp);
  digitalWrite(pulse,LOW);
}
void doublePulse(){
  // Double Pusle Welding
  digitalWrite(pulse,HIGH);
  delay(p1);
  digitalWrite(pulse,LOW);
  delay(dpd);
  digitalWrite(pulse,HIGH);
  delay(p2);
  digitalWrite(pulse,LOW);
}
int chkMax(int chk){
  // Error Checking Max Values
  if(screen==1){
    if(chk>max_sp){chk=max_sp;}
  }
  
  if(screen==2){
    if(menupos==0){if(chk>max_p){chk=max_p;}}
    if(menupos==1){if(chk>max_dpd){chk=max_dpd;}}
    if(menupos==2){if(chk>max_p){chk=max_p;}}
  }

  if(screen==3){
    if(menupos==0){if(chk>max_inc){chk=max_inc;}}
    if(menupos==1){if(chk>max_weldDelay){chk=max_weldDelay;}}
  }
  
  return chk;
}
int chkMin(int chk){
  // Error Checking Min Values
  if(screen==1){
    if(chk<min_sp){chk=min_sp;}
  }
  
  if(screen==2){
    if(menupos==0){if(chk<min_p){chk=min_p;}}
    if(menupos==1){if(chk<min_dpd){chk=min_dpd;}}
    if(menupos==2){if(chk<min_p){chk=min_p;}}
  }

  if(screen==3){
    if(menupos==0){if(chk<min_inc || chk==255){chk=min_inc;}}
    if(menupos==1){if(chk<min_weldDelay || chk>max_weldDelay){chk=min_weldDelay;}}
  }
  
  return chk;
}
float chkTemp(int pin){
  // Check Temp, Make Calc's and Return Value
  float vol = (analogRead(pin)/1024.0) * 5;
  float tempC = (vol - .5) * 100;
  return tempC;
}
void SAVE_SETTINGS()
{
  // Save Settings To EPROM
  
  CHK_SETTINGS(); // Check Settings Are Correct
  
  EEPROM.put(0,sp); 
  EEPROM.put(4,p1);
  EEPROM.put(8,dpd);
  EEPROM.put(12,p2);
  EEPROM.put(16,inc);
  EEPROM.put(20,weldDelay);
  EEPROM.put(24,playtone);
}
void LOAD_SETTINGS()
{
  // Load Settings From EPROM
  
  EEPROM.get(0,sp); 
  EEPROM.get(4,p1);
  EEPROM.get(8,dpd);
  EEPROM.get(12,p2);
  EEPROM.get(16,inc);
  EEPROM.get(20,weldDelay);
  EEPROM.get(24,playtone);

  CHK_SETTINGS(); // Check Settings Are Correct
  
}
void CHK_SETTINGS(){

  // Check Settings Are Correct
  
  if(isnan(sp)){sp=def_sp;}
  if(sp<min_sp){sp=def_sp;}
  if(sp>max_sp){sp=def_sp;}

  if(isnan(p1)){p1=def_p1;}
  if(p1<min_p){p1=def_p1;}
  if(p1>max_p){p1=def_p1;}

  if(isnan(dpd)){dpd=def_dpd;}
  if(dpd<min_dpd){dpd=def_dpd;}
  if(dpd>max_dpd){dpd=def_dpd;}

  if(isnan(p2)){p2=def_p2;}
  if(p2<min_p){p2=def_p2;}
  if(p2>max_p){p2=def_p2;}

  if(isnan(inc)){inc=def_inc;}
  if(inc<min_inc){inc=def_inc;}
  if(inc>max_inc){inc=def_inc;}

  if(isnan(weldDelay)){weldDelay=def_weldDelay;}
  if(weldDelay<min_weldDelay){weldDelay=def_weldDelay;}
  if(weldDelay>max_weldDelay){weldDelay=def_weldDelay;}
  
}
void DEF_SETTINGS(){

  // Load Default Settings
  
  sp = def_sp;
  
  p1 = def_p1;
  dpd = def_dpd;
  p2 = def_p2;

  inc = def_inc;
  weldDelay = def_weldDelay;
  playtone = def_playtone;
}

//===================================================================================
// WELDING OPERATION
//===================================================================================
void weldNow(){
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
//===================================================================================
// USER INPUT - TURN DETECTED
//===================================================================================
void uiTurnDetected(){
  switch (screen){
      case 0:
        if(up){
          menupos++;
          if (menupos>7){menupos=0;}
        }else{
          menupos--;
          if (menupos>7){menupos=7;}
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
          if(dpd>max_dpd){dpd=max_dpd;}
        }else{
          dpd=dpd-inc;
          if(dpd<min_dpd){dpd=min_dpd;}
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
            inc=chkMax(inc);
          }else{
            inc--;
            inc=chkMin(inc);
          }
      }
      if(menupos==1){
        if(up){
          weldDelay++;
          weldDelay=chkMax(weldDelay);
        }else{
          weldDelay--;
          weldDelay=chkMin(weldDelay);
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

//===================================================================================
// USER INPUT - BUTTON PRESSED
//===================================================================================
void uiButton(){
  
  switch (screen){
      case 0:
        if(menupos==0){screen=1;menupos=0;}
        if(menupos==1){screen=2;menupos=0;}
        if(menupos==2){screen=3;menupos=0;}

        if(menupos==3){screen=4;menupos=0;}
        if(menupos==4){screen=5;menupos=0;}
        if(menupos==5){screen=6;menupos=0;}

        if(menupos==6){screen=90;menupos=0;}
        if(menupos==7){screen=8;menupos=0;}
        
      break;

      case 1:
        screen=0;
        menupos=0;
      break;

      case 2:        
        if(menupos==3){
          screen=0;
          menupos=1;
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
          menupos=2;
        }else{
          if(menuopt){
            menuopt=false;
          }else{
            menuopt=true;
          }
        }
      break;

      case 4:
        screen=0;
        menupos=3;
      break;

      case 5:
        screen=0;
        menupos=4;
      break;

      case 6:
        screen=0;
        menupos=5;
      break;

      case 8:
        screen=0;
        menupos=7;
      break;
      
      case 90:
        screen=0;
        menupos=6;
      break;
    }

    draw();
    button=false;
}

//===================================================================================
// GUI / DISPLAY
//===================================================================================
void draw(){
  switch (screen){
    case 0:
      // MAIN MENU
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.setCursor(25,13); // Set Cursor
        u8g2.print("MAIN  MENU");
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
        u8g2.print("Save Settings");
        
        u8g2.setCursor(2,44); // Set Cursor
        if(menupos==4){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        u8g2.print("Load Settings");

        u8g2.setCursor(2,57); // Set Cursor
        if(menupos==5){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        u8g2.print("Default Settings");
      }
      if((menupos>5)&&(menupos<=8)){
        u8g2.setCursor(2,31); // Set Cursor
        if(menupos==6){u8g2.print(">");}
        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("Temperature");
        
        u8g2.setCursor(2,44); // Set Cursor
        if(menupos==7){u8g2.print(">");}
        u8g2.setCursor(10,44); // Set Cursor
        u8g2.print("About");

        u8g2.setCursor(2,57); // Set Cursor
        if(menupos==8){u8g2.print(">");}
        u8g2.setCursor(10,57); // Set Cursor
        //u8g2.print("About");
      }
        drawIcon(0,16,icoHome);
      } while ( u8g2.nextPage() ); // End Screen

      
    break;

    case 1:
      // SINGLE PULSE
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
      // DOUBLE PULSE
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
      // SETTINGS
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        //u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
        u8g2.setCursor(32,13); // Set Cursor
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

    case 4:
      // SAVE SETTINGS
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"SAVE");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(10,31); // Set Cursor
        //u8g2.print("iDIY Down Under");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Saving Settings...");
        u8g2.setCursor(22,60); // Set Cursor
        //u8g2.print("Settings...");
        //u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

      SAVE_SETTINGS();

      delay(750);

      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"SAVE");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(10,31); // Set Cursor
        //u8g2.print("iDIY Down Under");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Saving Settings...");
        u8g2.setCursor(2,60); // Set Cursor
        u8g2.print("DONE.");
        //u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

      delay(1500);

      screen=0;
      menupos=3;

      draw();

    break;

    case 5:
      // LOAD SETTINGS
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"LOAD");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(10,31); // Set Cursor
        //u8g2.print("iDIY Down Under");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Loading Settings...");
        u8g2.setCursor(22,60); // Set Cursor
        //u8g2.print("Settings...");
        //u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

      LOAD_SETTINGS();

      delay(750);

      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"LOAD");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(10,31); // Set Cursor
        //u8g2.print("iDIY Down Under");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Loading Settings...");
        u8g2.setCursor(2,60); // Set Cursor
        u8g2.print("DONE.");
        //u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

      delay(1500);

      screen=0;
      menupos=4;

      draw();

    break;

    case 6:
      // DEFAULT SETTINGS
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"DEFAULT");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(2,31); // Set Cursor
        u8g2.print("Applying Default");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Settings...");
        u8g2.setCursor(2,60); // Set Cursor
        //u8g2.print("Settings...");
        //u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

      DEF_SETTINGS();

      delay(750);

      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"DEFAULT");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(2,31); // Set Cursor
        u8g2.print("Applying Default");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Settings...");
        u8g2.setCursor(2,60); // Set Cursor
        u8g2.print("DONE.");
        //u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

      delay(1500);

      screen=0;
      menupos=5;

      draw();
      
    break;


    case 8:
      // ABOUT
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(40,13,"ABOUT");
        u8g2.drawHLine(2,18,124);

        u8g2.setCursor(10,31); // Set Cursor
        u8g2.print("iDIY Down Under");
        u8g2.setCursor(2,45); // Set Cursor
        u8g2.print("Spot Welder Control");
        u8g2.setCursor(22,60); // Set Cursor
        u8g2.print("Version: ");
        u8g2.print(ver);
        //if(menupos==1){drawIcon(112,64,icoBack);}
        drawIcon(0,16,icoBell);
        //drawIcon(112,16,icoBell);
      } while ( u8g2.nextPage() ); // End Screen

    break;

    case 90:
      // TEMPERATURE
      u8g2.firstPage(); // Start Screen
      do {
        u8g2.setFont(smlFont); // Set Font
        u8g2.drawStr(18,13,"TEMPERATURE");
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
        //drawIcon(112,16,icoESign);
        
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

//===================================================================================
// SETUP
//===================================================================================
void setup() {
  //
  // put your setup code here, to run once:
  //
  
  // Setup Pin Modes
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

  // Setup To Run
  u8g2.begin();  // Start LCD Display
  db=millis();  // Set Debonce
  tdb=millis(); // Set Debonce
  LOAD_SETTINGS(); // Load Settings From EPROM
  draw(); // Draw GUI
  
}

//===================================================================================
// LOOP
//===================================================================================
void loop() {
  //
  // put your main code here, to run repeatedly:  
  //

  if(weld){weldNow();}  // WELDING FLAGGED
  
  //
  // TEMP - Debounce
  //
  if(millis()-tdb>=1000){
  
    tdb=millis(); // Reset Debounce
  
    temp0=chkTemp(tempA);
    temp1=chkTemp(tempB);
  
    if (temp0<30 && temp1<30){
    //if ((temp0 + temp1)/2<30){
      digitalWrite(fan, LOW);
    } else {
      digitalWrite(fan, HIGH);
    }
  
    if (screen==90) {draw();}
  }
 
  //
  // BUTTON - Debunce
  //
  if(millis()-db>=250){
    if (digitalRead(sw) == LOW) {
      button = true;
      db=millis();  // Reset Debounce
    }
  }

  //
  // CHECK UI
  //
  if(button){uiButton();} // USER INPUT - BUTTON PRESSED
  if (TurnDetected) {uiTurnDetected();} // USER INPUT - TURN DETECTED
 
}
