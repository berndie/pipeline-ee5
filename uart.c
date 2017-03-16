/*
 * File:   uart.c
 * Author: Bernd
 *
 * Created on 16 maart 2017, 16:31
 */


#include <xc.h>
#include "uart.h"

unsigned char uart_receive_buffer[BUFFER_SIZE];
unsigned int uart_receive_buffer_index = 0;

void initUART1(void){
    //Init the UART1
    //TXSTA1bits.TX9 = 0;
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    //5 steps: see datasheet page 355
    TXSTA1bits.BRGH = 1;
    BAUDCON1bits.BRG16 = 1;
    /////1/////
    //Baud rate calculations:
    //SPBRGHx:SPBRGx =  ((Fosc/Desired Baud Rate)/64) - 1
    //Fosc = 48MHz
    //SPBRGHx:SPBRGx = 12
    //Datasheet page: 349-350
    SPBRGH1 = 0;
    SPBRG1 = 34;
    
    /////2/////
    //SYNC is default 0: Datasheet page 346
    TXSTA1bits.SYNC = 0;
    RCSTA1bits.SPEN = 1;
    //Datasheet page 347
    
    /////3/////
    PIE1bits.TXIE = 1;
    PIE1bits.RC1IE = 1;
    //Datasheet page 113
    
    /////4/////
    // page 357
    RCSTA1bits.CREN = 1;
}
void UARTReceive(char on_or_off){
    if(on_or_off == ON){
        RCSTA1bits.CREN = 1;
    }
    else{
        RCSTA1bits.CREN = 0;
    }
}
void clearUARTReceiveBuffer(void){
    for(int i = 0; i<BUFFER_SIZE; i++){
        uart_receive_buffer[i] = '\0';
    }
    uart_receive_buffer_index= 0 ;
}

