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

#pragma config XINST = OFF
#pragma config WDTEN = OFF
#pragma config CFGPLLEN = OFF
#pragma config OSC = INTOSCPLL
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)






//24Mhz/4 = 6Mhz --> 166ns per number 
const long DELAY = 120000;

const unsigned char getVerCommand[14] = "sys get ver\r\n";
//TODO fill in deveui, appeui, deveui
//void interrupt high_ISR(void);

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
    initUART1();
    initADC();
    initInterrupts();
    clearUARTReceiveBuffer();
    while(1){
        UARTReceive(ON);
        initLoRa();
        UARTReceive(OFF);     
        //sendUARTMessage(uart_receive_buffer);
        delay_ms(800);
        clearUARTReceiveBuffer();
        delay_ms(800);
        
        makeTempMessage(PIPE);
        sendUARTMessage(temp_display_message);
        delay_ms(800);
        
        makeTempMessage(AMBIENT);
        sendUARTMessage(temp_display_message);
        delay_ms(800);               
    }
}

void interrupt high_ISR(void){
    uart_interrupt();
    temperature_interrupt();
}