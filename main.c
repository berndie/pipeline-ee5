/*
 * File:   main.c
 * Author: Bernd
 *
 * Created on 17 februari 2017, 16:55
 */
#include <xc.h>

#pragma config XINST = OFF
#pragma config WDTEN = OFF
#pragma config CFGPLLEN = OFF
#pragma config OSC = INTOSCPLL
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)


#define BUFFER_SIZE 1000
#define TRUE 1
#define FALSE 0
#define PLUS_PIPE 0
#define MINUS_PIPE 1
#define PLUS_AMBIENT 2
#define MINUS_AMBIENT 3
#define PIPE 0
#define AMBIENT 1
#define ON 1
#define OFF 0



signed int plus_pipe;
signed int minus_pipe;
signed int plus_ambient;
signed int minus_ambient;
signed int temp_pipe;
signed int temp_ambient;
unsigned int currentChannel = PLUS_PIPE;    //defines which channel will be converted by the ADC (plus or minus terminal from temperature circuit))

void interrupt low_priority low_isr();   //low priority interrupt routine
void delay(void);
void initADC(void);

//24Mhz/4 = 6Mhz --> 166ns per number 
const long DELAY = 120000;

const unsigned char getVerCommand[14] = "sys get ver\r\n";
const unsigned char pipe_ascii[] = "Pipe";
const unsigned char ambient_ascii[] = "Amb.";

unsigned char uart_receive_buffer[BUFFER_SIZE];
unsigned char temp_display_message[] = "Pipe temp =      \r\n";
unsigned char asciiTemp[] = {' ',' ',' ',' ',' ',};

unsigned int uart_receive_buffer_index = 0;
void interrupt high_ISR(void);
bit isCommandSent;
unsigned char *currentMessagePointer;



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
void initADC(void){
    TRISAbits.TRISA0 = '1';
    TRISAbits.TRISA1 = '1';
    PORTAbits.RA0 = '0';
    PORTAbits.RA1 = '0';
    
    PORTBbits.RB0 = '0';
    TRISBbits.TRISB0 = '1';
    PORTBbits.RB1 = '0';
    TRISBbits.TRISB1 = '1';
	//PORTB = 0x00;
    //TRISB = 0x00;
    ANCON0 = 0xFC;  //Pin AN0 and AN1 are analog, the rest digital
    ANCON1 = 0x1C; //pin 8-12 are digital
    ADCON0 = 0x00;  //VSS, VDD, channel 00 (AN0), GO/DONE = 0 (idle), ADON = 1    clear ADCON0 to select channel 0 (AN0)
	ADCON1 = 0b10111110;    //Right justified, normal ADC operation, slowest acquisition time and conversion clock
  	ADCON0bits.ADON = 0x01;//Enable A/D module
    
    PIR1bits.ADIF = 0;  //clear ADC interrupt flag
    PIE1bits.ADIE = 1;  //enable ADC interrupt
    
    //IPR1bits.ADIP = 0;  //ADC is low priority
    ADCON0bits.GO_DONE = 1;
    
}

void fillInTemp(char pipe_or_ambience){
    signed int temp;
    if(pipe_or_ambience == PIPE){
        for(int i = 0; i<4; i++){
            temp_display_message[i] = pipe_ascii[i];   
        }
        temp = temp_pipe;
    }
    else{
        for(int i = 0; i<4; i++){
            temp_display_message[i] = ambient_ascii[i];   
        }
        temp = temp_ambient;
    }
    //int temp_0 = 25;
    //char temp = minus;
    if(temp < 0){
        asciiTemp[0] = '-';
        temp = - temp;
    }
    else{
        asciiTemp[0] = ' ';
    }
    asciiTemp[1] = temp/100 + 48; 
    asciiTemp[2] = temp % 100 / 10 + 48;
    asciiTemp[3] = temp % 10 + 48;
    
    
    // Fill in the tempDisplayCommand
    for(char i = 12; i<17; i++){
        temp_display_message[i] = asciiTemp[i-12];
    }
    
}

void UARTReceive(char on_or_off){
    if(on_or_off == ON){
        RCSTA1bits.CREN = 1;
    }
    else{
        RCSTA1bits.CREN = 0;
    }
}

void initInterrupts(void){
    INTCON = 0b11000000;    //enable global and peripheral interrupt
    RCONbits.IPEN = 0;  //disable priority interrupts
}

void clearUARTReceiveBuffer(void){
    for(int i = 0; i<BUFFER_SIZE; i++){
        uart_receive_buffer[i] = '\0';
    }
    uart_receive_buffer_index= 0 ;
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

void delay(void){
    long time = 0;
    while(time < DELAY){
        time++;
    }
}

signed int calculateTemp(int plus, int minus){
    signed int temp; 
    temp = plus - minus;
    temp = (int)((double)(temp) / 1024 * 2750 / 10);
    
    return temp;
}

void makeTempMessage(char pipe_or_ambient){
    if(pipe_or_ambient == PIPE){
        temp_pipe = calculateTemp(plus_pipe, minus_pipe);
        fillInTemp(PIPE);
    }
    else{
        temp_ambient = calculateTemp(plus_ambient, minus_ambient);
        fillInTemp(AMBIENT);
    }
    
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
        sendUARTMessage(getVerCommand);
        delay();
        UARTReceive(OFF);
        sendUARTMessage(uart_receive_buffer);
        delay();
        clearUARTReceiveBuffer();
        delay();
        
        makeTempMessage(PIPE);
        sendUARTMessage(temp_display_message);
        delay();
        
        makeTempMessage(AMBIENT);
        sendUARTMessage(temp_display_message);
        delay();                
    }
}

void interrupt high_ISR(void){
    
    //Interrupt UART
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
    
    if(PIR1bits.RC1IF == 1){
        PIR1bits.RC1IF = 0;
        uart_receive_buffer[uart_receive_buffer_index] = RCREG1;
        uart_receive_buffer_index += 1;
        if(uart_receive_buffer[uart_receive_buffer_index] == '\n'){
            RCSTA1bits.CREN = 0;
        }
        if(uart_receive_buffer_index > BUFFER_SIZE){
            clearUARTReceiveBuffer();
        }
    }
    //Interrupt AD
    if(PIR1bits.ADIF == 1)
    {
        PIR1bits.ADIF = 0;
        switch(currentChannel){
            case PLUS_PIPE:
                plus_pipe = ADRES;
                ADCON0bits.CHS = 0b1000;  //next channel is MINUS_PIPE = CH08
                currentChannel = MINUS_PIPE;
                break;
            case MINUS_PIPE:
                minus_pipe = ADRES;
                ADCON0bits.CHS = 0b0001;  //next channel is PLUS_AMBIENT = CH01
                currentChannel = PLUS_AMBIENT;
                break;
            case PLUS_AMBIENT:
                plus_ambient = ADRES;
                ADCON0bits.CHS = 0b0000;  //next channel is MINUS_AMBIENT = CH0
                currentChannel = MINUS_AMBIENT;
                break;
            case MINUS_AMBIENT:
                minus_ambient = ADRES;
                ADCON0bits.CHS = 0b1001;  //next channel is PLUS_PIE = CH09
                currentChannel = PLUS_PIPE;
                break;
        }
        ADCON0bits.GO_DONE = 1;
        
    }
}
