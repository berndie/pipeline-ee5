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
unsigned long int ind_value = 0 ;
unsigned int ind_measurement_old = 0;
unsigned int ind_measurement_new = 0;
unsigned long int cap_value;
unsigned int cap_measurement_old;
unsigned int cap_measurement_new;
float cap_freq;
float ind_freq;
float ind_freq2;

unsigned int  valueH = 0;
unsigned int  valueL = 0;
unsigned int  timer  = 0;
unsigned int interrupt_count_ind = 0;
unsigned int interrupt_count_cap = 0;

//unsigned int  inductive   = 0;
double percent = 0.0; 
unsigned char ind_frequency_message[200];
unsigned char cap_frequency_message[200];

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
       
        ind_measurement_old = ind_measurement_new; 
        ind_measurement_new = CCPR8L + 256*CCPR8H;
        ind_value = (ind_measurement_new + (interrupt_count_ind * 65535) - ind_measurement_old)/16;
        //if(ind_value  1600)
        ind_freq2 = ind_value*40*ind_value;
        ind_freq = 2000000/ind_value;
        
        interrupt_count_ind = 0;
         //valueH = CCPR7H;
         //valueL = CCPR7L;
         
         
         PIR4bits.CCP8IF=0;     //CLEAR interrupt flag when you are done!!!
     }
   if(PIR4bits.CCP4IF == 1)
     {
       
        cap_measurement_old = cap_measurement_new; 
        cap_measurement_new = (CCPR4L + 256*CCPR4H);     
        cap_value = (cap_measurement_new + (interrupt_count_cap * 65535) - cap_measurement_old)/16;
        cap_freq = 2000000/cap_value;
        
        interrupt_count_cap = 0;
        //cap_value = (cap_measurement2 - cap_measurement1)/16;
         //valueH = CCPR7H;
         //valueL = CCPR7L;
         
         
         PIR4bits.CCP4IF=0;     //CLEAR interrupt flag when you are done!!!
     }
    
}

void timer1_interrupt(void)
{
    if(PIR1bits.TMR1IF == 1)
    {
        interrupt_count_ind++;
        interrupt_count_cap++;
        PIR1bits.TMR1IF = 0;
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
    sprintf(ind_frequency_message,"The inductive frequency is: %f, %f, %lu \r\n", ind_freq, ind_freq2, ind_value);
    sprintf(cap_frequency_message,"The capacitive frequency is: %f\r\n",
            cap_freq);
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
    T1CONbits.TMR1CS0=0;
    T1CONbits.TMR1CS1=0;//select internal Fosc = 8Mhz 
    T1CONbits.T1CKPS1=0;
    T1CONbits.T1CKPS0=0;
    T1CONbits.T1OSCEN=0;
    TMR1L=0X00;
    TMR1H=0X00;
    CCPR8H=0x00;
    CCPR8L=0x00;
    //still need to set pr1 and some gate*/
    
    
 //CAPACITIVE
    TRISBbits.TRISB4 = 1;
    //select timer5
    CCPTMRS1bits.C4TSEL0=0;
    CCPTMRS1bits.C4TSEL1=0;
    PIE4bits.CCP4IE=1;
    PIR4bits.CCP4IF=0;
    CCP4CON=0x07;
    PIE1bits.TMR1IE = 1;
    
    T1CONbits.RD16=1;
    T1CONbits.TMR1ON=1;
    T1CONbits.TMR1CS0=0;
    T1CONbits.TMR1CS1=0;//select internal Fosc/4 = 2Mhz
    T1CONbits.T1CKPS1=0;
    T1CONbits.T1CKPS0=0;
    T1CONbits.T1OSCEN=0;
    TMR1L=0X00;
    TMR1H=0X00;
    
    CCPR4H=0x00;
    CCPR4L=0x00;
}
