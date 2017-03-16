/*
 * File:   delay.c
 * Author: Bernd
 *
 * Created on 17 maart 2017, 0:26
 */


#include <xc.h>
#include "delay.h"
void delay_ms(int ms){
    for(int i = 0; i<ms; i++){
    __delay_ms(1);
    }
}
