#include <TVout.h>
#include <fontALL.h>
#include <EEPROM.h>
#include <avr/io.h>        
#include <util/delay.h>
#include <avr/interrupt.h>  

#include "text.h"
#include "bmp.h"
#include "AccurateHeartX.h"

TVout tv;
byte oldCrosshairX, oldCrosshairY;
byte crosshairX, crosshairY;
byte switchCrosshair = 1;
byte textloop = 0;
byte crosshairchange = 0;
int DeviationX = 0;
int DeviationY = 0;
unsigned long duration;

void setup() {
  changeInputStandard();//切换输入制式需要关机后拨动开关，默认开关低电平为NTSC制式
  initOverlay();
  tv.select_font(font6x8);
  
  if(EEPROM.read(0)==255){
    EEPROM.write(0,switchCrosshair);
    EEPROM.write(1,DeviationX);
    EEPROM.write(2,DeviationY);   
  }
//初始化
  switchCrosshair=EEPROM.read(0);
  DeviationX=EEPROM.read(1);
  DeviationY=EEPROM.read(2);
//读取EEPROM

  valY = analogRead(A0)+DeviationY;
  valX = analogRead(A1)+DeviationX;
  valY = map(valY,DeviationY,1023+DeviationY,0,W);
  valX = map(valX,DeviationX,1023+DeviationX,0,H);

  oldvalY = valY;
  oldvalX = valX;

  crosshairX = valX;
  crosshairY = valY;

  oldCrosshairX = crosshairX;
  oldCrosshairY = crosshairY;

  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);

  tv.fill(0);
  
  MCUCR  |= (1 << ISC11) | (0 << ISC10);
  EICRA  |= (1 << ISC11) | (0 << ISC10);
  EIMSK  |= (1 << INT1); 
  sei();
  
}

void initOverlay() {
  TCCR1A = 0;
  // Enable timer1.  ICES0 is set to 0 for falling edge detection on input capture pin.
  TCCR1B = _BV(CS10);

  // Enable input capture interrupt
  TIMSK1 |= _BV(ICIE1);

  // Enable external interrupt INT0 on pin 2 with falling edge.
  EIMSK = _BV(INT0);
  EICRA = _BV(ISC01);
}

ISR(INT0_vect) {
  display.scanLine = 0;
}

ISR(INT1_vect){
   delay(10);
    if(digitalRead(3)==LOW)
     {
       duration = pulseIn(3, LOW);
       if(duration<=2000000){
         if(switchCrosshair>=1&&switchCrosshair<CROSSHAIRNUM)
           switchCrosshair++;
           else if(switchCrosshair==CROSSHAIRNUM)
           switchCrosshair=1;
           EEPROM.update(0, switchCrosshair);
           tv.fill(0);
           tv.delay_frame(1);
           delay(10);
       }//短按进入切换准心
       
       else if(duration>=5000000){
           DeviationY=511-analogRead(A0);
           DeviationX=511-analogRead(A1);
           EEPROM.write(1,DeviationX);
           EEPROM.write(2,DeviationY);//电位器输出的偏移值
           delay(10);
           tv.delay_frame(1);
       }
       //长按5秒进入校准
            while(digitalRead(3)==LOW)
           {
            delay(1);
           }
     }
}

void loop() {
  drawCrosshair();
  tv.delay_frame(1);
  getPotentiometer();
  
}

void drawCrosshair(){
  switch(switchCrosshair)
    {
     case 1:switchCrosshair==1;
            defaultCrosshair();
            break;
     case 2:switchCrosshair==2;
            Crosshair2();
            break;      
     case 3:switchCrosshair==3;
            Crosshair3();
            break; 
     case 4:switchCrosshair==4;
            Crosshair4();
            break; 
     case 5:switchCrosshair==5;
            Crosshair5();
            break; 
     case 6:switchCrosshair==6;
            Crosshair6();
            break;
     case 7:switchCrosshair==7;
            Crosshair7();
            break;            
     case CROSSHAIRNUM:switchCrosshair==CROSSHAIRNUM;
            emptyCrosshair();
            break;
    }
  }
  
void defaultCrosshair() {
  if(crosshairchange == 1){
  tv.draw_line(oldCrosshairX+27,oldCrosshairY,W,oldCrosshairY, 0);
  tv.draw_line(oldCrosshairX-27,oldCrosshairY,0,oldCrosshairY, 0);
  tv.draw_line(oldCrosshairX,oldCrosshairY+27,oldCrosshairX,H, 0);
  tv.draw_line(oldCrosshairX,oldCrosshairY-27,oldCrosshairX,0, 0);
  drawbmp(oldCrosshairX-26, oldCrosshairY-26, bmp1,0,0,0);
  }
  tv.draw_line(crosshairX+27,crosshairY,W,crosshairY, 1);
  tv.draw_line(crosshairX-27,crosshairY,0,crosshairY, 1);
  tv.draw_line(crosshairX,crosshairY+27,crosshairX,H, 1);
  tv.draw_line(crosshairX,crosshairY-27,crosshairX,0, 1);
  drawbmp(crosshairX-26, crosshairY-26, bmp1,0,0,0);

  if(crosshairchange == 1){
    erasebmp(oldCrosshairX-26,oldCrosshairY-26,bmp1,0,0,0);
    crosshairchange=0;
  }
}

void Crosshair2() {
  tv.draw_line(oldCrosshairX,oldCrosshairY,oldCrosshairX,H, 0);  
  tv.draw_line(oldCrosshairX+1,oldCrosshairY+1,oldCrosshairX,H, 0); 
  tv.draw_line(oldCrosshairX-1,oldCrosshairY+1,oldCrosshairX,H, 0);
  tv.draw_line(oldCrosshairX+16,oldCrosshairY,W,oldCrosshairY, 0);
  tv.draw_line(oldCrosshairX-16,oldCrosshairY,0,oldCrosshairY, 0);

  tv.draw_line(crosshairX,crosshairY,crosshairX,H, 1);
  tv.draw_line(crosshairX+1,crosshairY+1,crosshairX,H, 1);
  tv.draw_line(crosshairX-1,crosshairY+1,crosshairX,H, 1);
  tv.draw_line(crosshairX+16,crosshairY,W,crosshairY, 1);
  tv.draw_line(crosshairX-16,crosshairY,0,crosshairY, 1);
}

void Crosshair3() {
  tv.draw_line(oldCrosshairX-2,oldCrosshairY,0,oldCrosshairY, 1);
  tv.draw_line(oldCrosshairX+2,oldCrosshairY,W,oldCrosshairY, 1);
  tv.draw_line(oldCrosshairX,oldCrosshairY-2,oldCrosshairX,0, 1);
  tv.draw_line(oldCrosshairX,oldCrosshairY+2,oldCrosshairX,H, 1);
  tv.draw_line(oldCrosshairX+20,oldCrosshairY+1,W,oldCrosshairY+1, 0);
  tv.draw_line(oldCrosshairX+20,oldCrosshairY-1,W,oldCrosshairY-1, 0);
  tv.draw_line(oldCrosshairX-20,oldCrosshairY+1,0,oldCrosshairY+1, 0);
  tv.draw_line(oldCrosshairX-20,oldCrosshairY-1,0,oldCrosshairY-1, 0);
  tv.draw_line(oldCrosshairX+1,oldCrosshairY+20,oldCrosshairX+1,H, 0);
  tv.draw_line(oldCrosshairX+1,oldCrosshairY-20,oldCrosshairX+1,0, 0);
  tv.draw_line(oldCrosshairX-1,oldCrosshairY+20,oldCrosshairX-1,H, 0);
  tv.draw_line(oldCrosshairX-1,oldCrosshairY-20,oldCrosshairX-1,0, 0);

  tv.draw_line(oldCrosshairX-8,oldCrosshairY-1,oldCrosshairX-8,oldCrosshairY-3, 0);
  tv.draw_line(oldCrosshairX-8,oldCrosshairY+1,oldCrosshairX-8,oldCrosshairY+3, 0);
  tv.draw_line(oldCrosshairX+8,oldCrosshairY-1,oldCrosshairX-8,oldCrosshairY-3, 0);
  tv.draw_line(oldCrosshairX+8,oldCrosshairY+1,oldCrosshairX-8,oldCrosshairY+3, 0);
  tv.draw_line(oldCrosshairX-1,oldCrosshairY-8,oldCrosshairX-1,oldCrosshairY-8, 0);
  tv.draw_line(oldCrosshairX+1,oldCrosshairY-8,oldCrosshairX+1,oldCrosshairY-8, 0);
  tv.draw_line(oldCrosshairX-1,oldCrosshairY+8,oldCrosshairX-1,oldCrosshairY+8, 0);
  tv.draw_line(oldCrosshairX+1,oldCrosshairY+8,oldCrosshairX+1,oldCrosshairY+8, 0);

  tv.draw_line(oldCrosshairX-20,oldCrosshairY-2,oldCrosshairX-20,oldCrosshairY-5, 0);
  tv.draw_line(oldCrosshairX-20,oldCrosshairY+2,oldCrosshairX-20,oldCrosshairY+5, 0);
  tv.draw_line(oldCrosshairX+20,oldCrosshairY-2,oldCrosshairX-20,oldCrosshairY-5, 0);
  tv.draw_line(oldCrosshairX+20,oldCrosshairY+2,oldCrosshairX-20,oldCrosshairY+5, 0);
  tv.draw_line(oldCrosshairX-2,oldCrosshairY-20,oldCrosshairX-5,oldCrosshairY-20, 0);
  tv.draw_line(oldCrosshairX+2,oldCrosshairY-20,oldCrosshairX+5,oldCrosshairY-20, 0);
  tv.draw_line(oldCrosshairX-2,oldCrosshairY+20,oldCrosshairX-5,oldCrosshairY+20, 0);
  tv.draw_line(oldCrosshairX+2,oldCrosshairY+20,oldCrosshairX+5,oldCrosshairY+20, 0);
  
  
  
  
  tv.draw_line(crosshairX-2,crosshairY,0,crosshairY, 1);
  tv.draw_line(crosshairX+2,crosshairY,W,crosshairY, 1);
  tv.draw_line(crosshairX,crosshairY-2,crosshairX,0, 1);
  tv.draw_line(crosshairX,crosshairY+2,crosshairX,H, 1);
  tv.draw_line(crosshairX+20,crosshairY+1,W,crosshairY+1, 1);
  tv.draw_line(crosshairX+20,crosshairY-1,W,crosshairY-1, 1);
  tv.draw_line(crosshairX-20,crosshairY+1,0,crosshairY+1, 1);
  tv.draw_line(crosshairX-20,crosshairY-1,0,crosshairY-1, 1);  
  tv.draw_line(crosshairX+1,crosshairY+20,crosshairX+1,H, 1);
  tv.draw_line(crosshairX+1,crosshairY-20,crosshairX+1,0, 1);
  tv.draw_line(crosshairX-1,crosshairY+20,crosshairX-1,H, 1);
  tv.draw_line(crosshairX-1,crosshairY-20,crosshairX-1,0, 1);

  tv.draw_line(crosshairX-8,crosshairY-1,crosshairX-8,crosshairY-3, 1);
  tv.draw_line(crosshairX-8,crosshairY+1,crosshairX-8,crosshairY+3, 1);
  tv.draw_line(crosshairX+8,crosshairY-1,crosshairX+8,crosshairY-3, 1);
  tv.draw_line(crosshairX+8,crosshairY+1,crosshairX+8,crosshairY+3, 1);
  tv.draw_line(crosshairX-1,crosshairY-8,crosshairX-1,crosshairY-8, 1);
  tv.draw_line(crosshairX+1,crosshairY-8,crosshairX+1,crosshairY-8, 1);
  tv.draw_line(crosshairX-1,crosshairY+8,crosshairX-1,crosshairY+8, 1);
  tv.draw_line(crosshairX+1,crosshairY+8,crosshairX+1,crosshairY+8, 1);

  tv.draw_line(crosshairX-20,crosshairY-2,crosshairX-20,crosshairY-5, 1);
  tv.draw_line(crosshairX-20,crosshairY+2,crosshairX-20,crosshairY+5, 1);
  tv.draw_line(crosshairX+20,crosshairY-2,crosshairX+20,crosshairY-5, 1);
  tv.draw_line(crosshairX+20,crosshairY+2,crosshairX+20,crosshairY+5, 1);
  tv.draw_line(crosshairX-2,crosshairY-20,crosshairX-5,crosshairY-20, 1);
  tv.draw_line(crosshairX+2,crosshairY-20,crosshairX+5,crosshairY-20, 1);
  tv.draw_line(crosshairX-2,crosshairY+20,crosshairX-5,crosshairY+20, 1);
  tv.draw_line(crosshairX+2,crosshairY+20,crosshairX+5,crosshairY+20, 1);
}

void Crosshair4() {
  if(crosshairchange == 1){  
  tv.draw_line(oldCrosshairX+27,oldCrosshairY,W,oldCrosshairY, 0);
  tv.draw_line(oldCrosshairX-27,oldCrosshairY,0,oldCrosshairY, 0);
  tv.draw_line(oldCrosshairX,oldCrosshairY+27,oldCrosshairX,H, 0);
  tv.draw_line(oldCrosshairX,oldCrosshairY-27,oldCrosshairX,0, 0);
  drawbmp(oldCrosshairX-26, oldCrosshairY-26, bmp4,0,0,0);
  }
  tv.draw_line(crosshairX+27,crosshairY,W,crosshairY, 1);
  tv.draw_line(crosshairX-27,crosshairY,0,crosshairY, 1);
  tv.draw_line(crosshairX,crosshairY+27,crosshairX,H, 1);
  tv.draw_line(crosshairX,crosshairY-27,crosshairX,0, 1);
  drawbmp(crosshairX-26, crosshairY-26, bmp4,0,0,0);

  if(crosshairchange == 1){
    erasebmp(oldCrosshairX-26,oldCrosshairY-26,bmp4,0,0,0);
    crosshairchange=0;
  }
}

void Crosshair5() {
  tv.set_pixel(oldCrosshairX, oldCrosshairY-3, 0);
  tv.set_pixel(oldCrosshairX, oldCrosshairY-2, 0);
  tv.set_pixel(oldCrosshairX, oldCrosshairY+2, 0);
  tv.set_pixel(oldCrosshairX, oldCrosshairY+3, 0);

  tv.set_pixel(oldCrosshairX-3, oldCrosshairY, 0);
  tv.set_pixel(oldCrosshairX-2, oldCrosshairY, 0);
  tv.set_pixel(oldCrosshairX+2, oldCrosshairY, 0);
  tv.set_pixel(oldCrosshairX+3, oldCrosshairY, 0);

  tv.set_pixel(crosshairX, crosshairY-3, 1);
  tv.set_pixel(crosshairX, crosshairY-2, 1);
  tv.set_pixel(crosshairX, crosshairY+2, 1);
  tv.set_pixel(crosshairX, crosshairY+3, 1);
  
  tv.set_pixel(crosshairX-3, crosshairY, 1);
  tv.set_pixel(crosshairX-2, crosshairY, 1);
  tv.set_pixel(crosshairX+2, crosshairY, 1);
  tv.set_pixel(crosshairX+3, crosshairY, 1);
}

void Crosshair6() {
  if(crosshairX >= W-16)
  crosshairX=W-16;
  else if(crosshairX <= 16)
  crosshairX=16;
  else if(crosshairY <= 16 )
  crosshairY=16;
  
  tv.set_pixel(oldCrosshairX, oldCrosshairY, 0);
  tv.draw_circle(oldCrosshairX,oldCrosshairY,16,0);
  
  
  tv.set_pixel(crosshairX, crosshairY, 1);
  tv.draw_circle(crosshairX,crosshairY,16,1);
  
}

void Crosshair7() {
  tv.draw_line(oldCrosshairX,oldCrosshairY,oldCrosshairX-3,oldCrosshairY+3, 0);
  tv.draw_line(oldCrosshairX+1,oldCrosshairY+1,oldCrosshairX+3,oldCrosshairY+3, 0);
  
  tv.draw_line(crosshairX,crosshairY,crosshairX-3,crosshairY+3, 1);
  tv.draw_line(crosshairX+1,crosshairY+1,crosshairX+3,crosshairY+3, 1);
}

void emptyCrosshair(){
  if(textloop = 0){
    textloop++;
    tv.delay_frame(1);
    tv.print(17, 87, textpuremode);
    delay(1000);
    }
  }

void getPotentiometer(){
  valY = analogRead(A0)+DeviationY;
  valX = analogRead(A1)+DeviationX;
  valX = map(valX,DeviationX,1023+DeviationX,0,W);
  valY = map(valY,DeviationY,1023+DeviationY,0,H);
  
  oldCrosshairX = crosshairX;
  oldCrosshairY = crosshairY;
  
  
  if (valX!=-1) {
      crosshairX = valX;}
      if (crosshairX < MINX) {
  crosshairX = MINX;
      }
      if (crosshairX > MAXX) {
  crosshairX = MAXX;
      }

  if (valY!=-1) {
      crosshairY = valY;}
      if (crosshairY < MINY) {
  crosshairY = MINY;
      }
      if (crosshairY > MAXY) {
  crosshairY = MAXY;
      }

if(oldvalY != valY||oldvalX != valX)
    crosshairchange = 1;


  oldvalY = valY;
  oldvalX = valX;

  }

void changeInputStandard()
{
 if(digitalRead(4)==HIGH)
 tv.begin(NTSC, W, H);
 else if(digitalRead(4)==LOW) 
 tv.begin(PAL, W, H);
}
 
void drawbmp(int x, int y, const unsigned char * bmp,
           uint16_t i, uint8_t width, uint8_t lines) {

  uint8_t temp, lshift, rshift, save, xtra;
  uint16_t si = 0;

  rshift = x&7;
  lshift = 8-rshift;

  if (width == 0) {
    width = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }
  if (lines == 0) {
    lines = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }
    
  if (width&7) {
    xtra = width&7;
    width = width/8;
    width++;
  } else {
    xtra = 8;
    width = width/8;
  }
  
  for (uint8_t l = 0; l < lines; l++) {
    if (((y+l) < 0) || ((y+l) >= display.vres)) {
      i += width;
      continue;
    }
    si = (y + l)*display.hres + x/8;
    if (width == 1)
      temp = 0xff >> rshift + xtra;
    else
      temp = 0;
    temp = pgm_read_byte((uint32_t)(bmp) + i++);
    boolean wrapped = false;
    if (x >= 0) {
      display.screen[si++] |= temp >> rshift;
      for (uint16_t b = i + width-1; i < b; i++) {
  if (!wrapped) {
    display.screen[si] |= temp << lshift;
    temp = pgm_read_byte((uint32_t)(bmp) + i);
    display.screen[si] |= temp >> rshift;
  }
  if (((si) / display.hres) < ((si+1) / display.hres)) {
    wrapped = true;
  }
  si++;
      }
      if (!wrapped) {
  display.screen[si] |= temp << lshift;
      }
    } else {
      // x < 0
      if (rshift == 0) {
  si++;
      }
      wrapped = true;
      for (uint16_t b = i + width-1; i < b; i++) {
  if (wrapped && ((si / display.hres) != ((si-1) / display.hres))) {
    wrapped = false;
  }
  if (!wrapped) {
    display.screen[si] |= temp << lshift;
    temp = pgm_read_byte((uint32_t)(bmp) + i);
    display.screen[si++] |= temp >> rshift;
  } else {
    si++;
    temp = pgm_read_byte((uint32_t)(bmp) + i);
  }
      }
      display.screen[si] |= temp << lshift;

    }
  }
} 

void erasebmp(int x, int y, const unsigned char * bmp,
           uint16_t i, uint8_t width, uint8_t lines) {

  uint8_t temp, lshift, rshift, xtra;
  uint16_t si = 0;
  
  rshift = x&7;
  lshift = 8-rshift;
  if (width == 0) {
    width = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }
  if (lines == 0) {
    lines = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }
    
  if (width&7) {
    xtra = width&7;
    width = width/8;
    width++;
  } else {
    xtra = 8;
    width = width/8;
  }
  
  for (uint8_t l = 0; l < lines; l++) {
    if (((y+l) < 0) || ((y+l) >= display.vres)) {
      i += width;
      continue;
    }
    si = (y + l)*display.hres + x/8;
    if (width == 1)
      temp = 0xff >> rshift + xtra;
    else
      temp = 0;
    temp = pgm_read_byte((uint32_t)(bmp) + i++);
    boolean wrapped = false;
    if (x >= 0) {
      display.screen[si++] &= ~(temp >> rshift);
      for (uint16_t b = i + width-1; i < b; i++) {
  if (!wrapped) {
    display.screen[si] &= ~(temp << lshift);
    temp = pgm_read_byte((uint32_t)(bmp) + i);
    display.screen[si] &= ~(temp >> rshift);
  }
  if (((si) / display.hres) < ((si+1) / display.hres)) {
    wrapped = true;
  }
  si++;
      }
      if (!wrapped) {
  if (rshift + xtra - 8 > 0)
    display.screen[si] &= ~(temp << lshift);
      }
    } else {
      // x < 0
      if (rshift == 0) {
  si++;
      }
      wrapped = true;
      for (uint16_t b = i + width-1; i < b; i++) {
  if (wrapped && ((si / display.hres) != ((si-1) / display.hres))) {
    wrapped = false;
  }
  if (!wrapped) {
    display.screen[si] &= ~(temp << lshift);
    temp = pgm_read_byte((uint32_t)(bmp) + i);
    display.screen[si++] &= ~(temp >> rshift);
  } else {
    si++;
    temp = pgm_read_byte((uint32_t)(bmp) + i);
  }
      }
      if (rshift + xtra - 8 > 0)
  display.screen[si] &= ~(temp << lshift);
    }
  }
}




