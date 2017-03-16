/*
 * File:   lora.c
 * Author: Bernd
 *
 * Created on 16 maart 2017, 22:03
 */


#include <xc.h>
#include "uart.h"
#include "delay.h"
const unsigned char set_dev_eui[] = "mac set deveui 0004A30B001ACF7D\r\n";
const unsigned char set_app_eui[] = "mac set appeui 74C542453857974A\r\n";
const unsigned char set_app_key[] = "mac set appkey 2E15AFC94A7EF69AE61A814737C38699\r\n";

//TODO choose datarate, is dependent of packet size
const unsigned char set_data_rate[] = "mac set dr x\r\n";

const unsigned char join_network[]= "mac join otaa\r\n";

void initLoRa(void){
    sendUARTMessage(set_dev_eui);
    delay_ms(800);
    sendUARTMessage(set_app_eui);
    delay_ms(800);
    sendUARTMessage(set_app_key);
    delay_ms(800);
    sendUARTMessage(join_network);
    delay_ms(800);
}
