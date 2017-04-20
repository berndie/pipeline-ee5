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
#include "digipot.h"

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
    initUART1();
    initADC();
    initInterrupts();
    initSPI();
    init_frequency();
    initLoRa();
    
    while(1){
        UARTReceive(ON);
        sendSensorData(1.1,2.2,3.3,4.4);
        UARTReceive(OFF);     
        //sendUARTMessage(uart_receive_buffer);
        delay_ms(800);
        make_frequency_message();
        
        make_step_message();
        sendUARTMessage(ind_frequency_message);
        delay_ms(800);
        sendUARTMessage(cap_frequency_message);
        delay_ms(800);
        sendUARTMessage(cap_step_message);
        delay_ms(800);
        
        //makeTempMessage(PIPE);
        //sendUARTMessage(temp_display_message);
        delay_ms(800);
        
        //makeTempMessage(AMBIENT);
        //sendUARTMessage(temp_display_message);
        delay_ms(800);
        
        // SPI for the digipot
        digipot();
    }
}

void interrupt high_ISR(void){
    temperature_interrupt();
    uart_interrupt();
    frequency_interrupt();
    timer1_interrupt();
}