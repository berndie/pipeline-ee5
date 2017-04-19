/*
 * File:   frequency.c
 * Author: Bernd
 *
 * Created on 23 maart 2017, 16:11
 */


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * Prototypes
 */
void frequency_interrupt(void);   //high priority interrupt routine
//low priority interrupt routine, not used in this example
void init_frequency(void);



/*
 * Global Variables
 */
unsigned int ind_value = 0 ;
unsigned int cap_value = 0 ;
unsigned int ind_measurement1 = 0;
unsigned int ind_measurement2 = 0;
unsigned int cap_measurement1 = 0;
unsigned int cap_measurement2 = 0;
unsigned int  valueH = 0;
unsigned int  valueL = 0;
unsigned int  timer  = 0;
unsigned int  inductive   = 0;
double percent = 0.0; 
char ind_frequency_message[] = "The inductive frequency is:      \r\n";
char cap_frequency_message[] = "The capacitive frequency is:      \r\n";
/*
 * Interrupt Service Routines
 */
/********************************************************* 
	Interrupt Handler
**********************************************************/
void frequency_interrupt(void)
{
	if(PIR4bits.CCP8IF == 1)
     {
       
        ind_measurement1 = ind_measurement2; 
        ind_measurement2 = CCPR8L + 256*CCPR8H;
        if((ind_measurement2 - ind_measurement1)/16 < 1600){
            ind_value = (ind_measurement2 - ind_measurement1)/16;
        }
         //valueH = CCPR7H;
         //valueL = CCPR7L;
         
         
         PIR4bits.CCP8IF=0;     //CLEAR interrupt flag when you are done!!!
     }
    if(PIR4bits.CCP7IF == 1)
     {
       
        cap_measurement1 = cap_measurement2; 
        cap_measurement2 = CCPR7L + 256*CCPR7H;
        if((cap_measurement2 - cap_measurement1)/16 < 1600){    //capturing every 16th edge
            cap_value = (cap_measurement2 - cap_measurement1)/16;
        }
         //valueH = CCPR7H;
         //valueL = CCPR7L;
         
         
         PIR4bits.CCP7IF=0;     //CLEAR interrupt flag when you are done!!!
     }
    
}

/*
 * Functions
 */
 /*************************************************
			Main
**************************************************/
//void main(void)
//{
//   init_frequency();
//   
//    while(1)    //Endless loop
//    {
//        inductive= 40*timer*timer;
//        //nano Henry //
//        
//        if((inductive>1000)&(inductive<10000000))
//        {
//           percent = inductive /10000000; 
//        }
//        
//    }
//}

void make_frequency_message(){ 
    ind_frequency_message[29] = (ind_value / 1000 + 48); 
    ind_frequency_message[30] =  (ind_value % 1000 / 100 + 48);
    ind_frequency_message[31] =  (ind_value % 100 / 10 + 48);
    ind_frequency_message[32] =  (ind_value % 10 + 48);
    cap_frequency_message[30] = (cap_value / 1000 + 48); 
    cap_frequency_message[31] =  (cap_value % 1000 / 100 + 48);
    cap_frequency_message[32] =  (cap_value % 100 / 10 + 48);
    cap_frequency_message[33] =  (cap_value % 10 + 48);
}

/*************************************************
			Initialize the CHIP
**************************************************/
void init_frequency(void)
{
    //INDUCTIVE
    
    TRISCbits.TRISC1 = 1;
    //select timer1
    CCPTMRS2bits.C8TSEL1=0;
    CCPTMRS2bits.C8TSEL0=0;
    //CCP capture mode; 
    PIE4bits.CCP8IE=1;
    PIR4bits.CCP8IF=0;
    CCP8CON=0x07; //00000100  
    // Set interrupt enable
	INTCONbits.GIE = 1;
    //PRE=1:1 enable Timer1 ("Always On" mode)	
    T1CONbits.RD16=1;
    T1CONbits.TMR1ON=1;
    T1CONbits.TMR1CS0=1;
    T1CONbits.TMR1CS1=0;//select internal Fosc = 8Mhz 
    T1CONbits.T1CKPS1=0;
    T1CONbits.T1CKPS0=0;
    T1CONbits.T1OSCEN=0;
    TMR1L=0X00;
    TMR1H=0X00;
    CCPR8H=0x00;
    CCPR8L=0x00;
    //still need to set pr1 and some gate
    
    
    //CAPACITIVE
    TRISBbits.TRISB7 = 1;
    //select timer5
    CCPTMRS1bits.C7TSEL0=0;
    CCPTMRS1bits.C7TSEL1=1;
    PIE4bits.CCP7IE=1;
    PIR4bits.CCP7IF=0;
    CCP7CON=0x07;
    
    T5CONbits.RD16=1;
    T5CONbits.TMR5ON=1;
    T5CONbits.TMR5CS0=1;//select internal Fosc = 8Mhz 
    T5CONbits.TMR5CS1=0;
    T5CONbits.T5CKPS1=0;//prescale 1:1
    T5CONbits.T5CKPS0=0;
    T5CONbits.T5OSCEN=0;
    
    TMR5L=0X00;
    TMR5H=0X00;
    CCPR7H=0x00;
    CCPR7L=0x00;

}