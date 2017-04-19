/*
 * File:   DigipotSPI.c
 * Author: gil
 *
 * Created on 3 maart 2017, 15:42
 */
#pragma config XINST = OFF
#include <math.h>
#include <xc.h>
#include "digipot_spi.h"

char step;
float nominalfrequency;
char state=0x00;


 

void main(void) {
    initSPI();
    while(1){
    }
}

void initSPI(){
    SSP2STAT = 0b00000000;                                  //configure the pic for spi communication
    SSP2CON1 = 0b00110010;
    TRISB = 0x01;
    TRISC = 0x00;
    TRISA = 0x01;
    PORTBbits.RB2=1;
    ADCON1 = 0x0F;
    remappings();                                            //remap the pins
    nominalfrequency = 1.44/((1000+2*step*390.63)*0.000288 * pow(0.9384, step));//change the nominal frequency to 5Hz
}

void remappings(){
    EECON2 = 0x55;                                          //unlock the pins
    EECON2 = 0xaa;
    IOLOCK = 0;
    RPINR21 = 0x08;                                         //assign sdi2 to pin 26
    RPOR7 = 0x0b;                                           //assign sck2 to pin 25
    RPOR9 = 0x0a;                                           //assign sdo2 to pin 27
    
}
void checkFrequency(){
    //check the current frequency witha ccp module and store it in the varible frequency
}

void adjustDigipot(){

   PORTBbits.RB3=0;
if(state==0x00){
    SSPBUF = 0x00;                                              //send a write command to the digipot
    state++;                                                    
}
if(SSP2STATbits.BF==1){
   if(bigorsmall==0x01){                                    //if the frequency is too big
       step++;                                              //increase step
   }
   if(bigorsmall==0x02){                                    //if the frequency is too small
       step--;                                              //decrease step
   }    
    SSPBUF = step;                                              //send step to the digipot   
    state=0x00;
    nominalfrequency = 1.44/((1000+2*step*390.63)*0.000288 * pow(0.9384, step));
    }
}

void unselect(){
  PORTBbits.RB3=1;  
}