/*
 * File:   DigipotSPI.c
 * Author: gil
 *
 * Created on 3 maart 2017, 15:42
 */
#pragma config XINST = OFF
#include <math.h>
#include <xc.h>
#include <stdio.h>
#include "frequency.h"

char step = 0x0;
float nominalfrequency;
char state=0x00;
float frequency;
char bigorsmall;
unsigned char cap_step_message[200];


void initSPI();
void remappings();
void adjustDigipot();
void adjustDigipot();
void unselect();
void make_step_message();

void initSPI(){
    SSP2STAT = 0b00000000;                                  //configure the pic for spi communication
    SSP2CON1 = 0b00110010;
    TRISBbits.TRISB2 = 0;
    TRISAbits.TRISA5 = 0;
    TRISBbits.TRISB5 = 1;
    TRISAbits.TRISA2 = 0;
    remappings();                                            //remap the pins
    nominalfrequency = 1.44/((1000+2*step*390.63)*0.000288 * pow(0.9384, step));//change the nominal frequency to 5Hz
}

void remappings(){
    EECON2 = 0x55;                                          //unlock the pins
    EECON2 = 0xaa;
    IOLOCK = 0;
    RPINR21 = 0x08;                                         //assign sdi2 to pin 26
    RPOR2 = 0x0b;                                           //assign sck2 to pin 7
    RPOR5 = 0x0a;                                           //assign sdo2 to pin 23 ->see page 174
    
}

void make_step_message(void)
{
    sprintf(cap_step_message,"The digipot step is: %u \r\n",
            step);
}

void digipot(){
        frequency = (float) cap_value;

    if (frequency > 1.033*nominalfrequency ){               //if the measured frequency is too high
        bigorsmall = 0x01;                                  //too big
        adjustDigipot();
    }
    if (frequency< 0.9692*nominalfrequency&&step>0){        //if the measured frequency is too low
        bigorsmall =0x02;                                   //too small
        adjustDigipot();
    }
    else{
        if(PORTAbits.RA2==0){                               //if there's nothing to send and the /cs is still low
            unselect();                                         // /cs=1
        }
    }
}

void adjustDigipot(){

   PORTAbits.RA2=0;
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
  PORTAbits.RA2=1;  
}