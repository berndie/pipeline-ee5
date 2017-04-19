/*
 * File:   main.c
 * Author: Bernd
 *
 * Created on 17 februari 2017, 16:55
 */
#include <xc.h>
#include "uart.h"
#include "temperature.h"
#include "lora.h"
#include "delay.h"
#include "frequency.h"
#include "digipot_spi.h"

#pragma config XINST = OFF
#pragma config WDTEN = OFF
#pragma config CFGPLLEN = OFF
#pragma config OSC = INTOSCPLL
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)

void initInterrupts(void){
    INTCON = 0b11000000;    //enable global and peripheral interrupt
    RCONbits.IPEN = 0;  //disable priority interrupts
}

void main(void) {
    // 8Mhz mode
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS0 = 1;
    OSCCONbits.SCS1 = 1;
    //When started, a new command can be sent
    isCommandSent = TRUE;
    init_frequency();
    initUART1();
    initADC();
    initInterrupts();
    while(1){
        inductive= 40*value*value;
        UARTReceive(ON);
        initLoRa();
        UARTReceive(OFF);     
        //sendUARTMessage(uart_receive_buffer);
        delay_ms(800);
        make_frequency_message();
        sendUARTMessage(frequency_message);
        delay_ms(800);
        
        makeTempMessage(PIPE);
        sendUARTMessage(temp_display_message);
        delay_ms(800);
        
        makeTempMessage(AMBIENT);
        sendUARTMessage(temp_display_message);
        delay_ms(800);
        
        // SPI for the digipor
        checkFrequency();
        if (frequency > 1.033*nominalfrequency ){               //if the measured frequency is too high
            bigorsmall = 0x01;                                  //too big
            adjustDigipot();
        }
        if (frequency< 0.9692*nominalfrequency&&step>0){        //if the measured frequency is too low
            bigorsmall =0x02;                                   //too small
            adjustDigipot();
        }
        else{
            if(PORTBbits.RB3==0){                               //if there's nothing to send and the /cs is still low
                unselect();                                         // /cs=1
            }
        }
    }
}

void interrupt high_ISR(void){
    temperature_interrupt();
    uart_interrupt();
    frequency_interrupt();
}