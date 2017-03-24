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
unsigned int value = 0 ;
unsigned int measurement1 = 0;
unsigned int measurement2 = 0;
unsigned int  valueH = 0;
unsigned int  valueL = 0;
unsigned int  timer  = 0;
unsigned int  inductive   = 0;
double percent = 0.0; 
char frequency_message[] = "The frequency is:      \r\n";
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
       
        measurement1 = measurement2; 
        measurement2 = CCPR8L + 256*CCPR8H;
        if((measurement2 - measurement1)/16 < 1600){
            value = (measurement2 - measurement1)/16;
        }
         //valueH = CCPR7H;
         //valueL = CCPR7L;
         
         
         PIR4bits.CCP8IF=0;     //CLEAR interrupt flag when you are done!!!
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
    frequency_message[19] = (value / 1000 + 48); 
    frequency_message[20] =  (value % 1000 / 100 + 48);
    frequency_message[21] =  (value % 100 / 10 + 48);
    frequency_message[22] =  (value % 10 + 48);
}

/*************************************************
			Initialize the CHIP
**************************************************/
void init_frequency(void)
{
    TRISCbits.TRISC1 = 1;
    //seclect timer1
    CCPTMRS2bits.C8TSEL1=0;
    CCPTMRS2bits.C8TSEL0=0;
    //CCP mode; 
    PIE4bits.CCP8IE =1;
    PIR4bits.CCP8IF=0;
    //timer
    //PIR2bits.TMR3IF = 0;
    //PIE2bits.TMR3IE = 1;
    // Set clock source to Fosc/4 (to work with special event trigger) - 2MHz
    // Set trigger on rising edge
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
}