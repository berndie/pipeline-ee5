/*
 * File:   uart.c
 * Author: Bernd
 *
 * Created on 16 maart 2017, 16:31
 */


#include <xc.h>
#include "uart.h"



char isCommandSent = TRUE;
unsigned char *currentMessagePointer;
unsigned char uart_receive_buffer[BUFFER_SIZE];
unsigned int uart_receive_buffer_index = 0;

void initUART1(void){
    //Init the UART1
    
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    //5 steps: see datasheet page 355
    TXSTA1bits.BRGH = 1;
    BAUDCON1bits.BRG16 = 1;
    /////1/////
    //Baud rate calculations:
    //SPBRGHx:SPBRGx =  ((Fosc/Desired Baud Rate)/4) - 1
    //Fosc = 8MHz
    //Desired Baud Rate = 57600
    //SPBRGHx:SPBRGx = 34
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
void sendUARTMessage(unsigned char *newMessagePointer){
    // Check if previous message is sent
    if(isCommandSent == TRUE){     
        //Change the current message
        currentMessagePointer = newMessagePointer;
        //The new message isn't sent yet
        isCommandSent == FALSE;
        //If TXEN is set to 1 --> TX1IF is set implicitly
        //!!!!! THIS MEANS AN INTERRUPT IS TRIGGERED IF GIE AND PIE = 1
        PIE1bits.TXIE = 1;
        TXSTA1bits.TXEN = 1;
        //Datasheet page 246
    }
}
void clearUARTReceiveBuffer(void){
    for(int i = 0; i<BUFFER_SIZE; i++){
        uart_receive_buffer[i] = '\0';
    }
    uart_receive_buffer_index= 0 ;
}
    
void uart_interrupt(void){
    //Interrupt for the receiving part
    if(PIR1bits.RC1IF == 1){
        PIR1bits.RC1IF = 0;
        // Save the received byte in the receive buffer
        uart_receive_buffer[uart_receive_buffer_index] = RCREG1;
        uart_receive_buffer_index += 1;
        // If there is overflow, clear the buffer
        // this is only for emergencies, buffer needs to be cleared with
        // clearUARTReceiveBuffer
        if(uart_receive_buffer_index > BUFFER_SIZE){
            clearUARTReceiveBuffer();
        }
    }
    
    //Interrupt for the sending part
    if(PIR1bits.TX1IF == 1){
        //If end of string is reached --> stop transmitting
        if(*currentMessagePointer == '\0'){
            PIE1bits.TXIE = 0;
            isCommandSent = TRUE;
        }
        else{
            //Prepare next byte for sending
            TXREG1 = *currentMessagePointer;
            //Go to the next byte
            currentMessagePointer += 1;
        }
    }
}
