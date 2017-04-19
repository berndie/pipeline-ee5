/*
 * File:   lora.c
 * Author: Bernd
 *
 * Created on 16 maart 2017, 22:03
 */


#include <xc.h>
#include "uart.h"
#include "delay.h"
#include <stdio.h>

const unsigned char set_dev_eui[] = "mac set deveui 0004A30B001ACF7D\r\n";
const unsigned char set_app_eui[] = "mac set appeui 74C542453857974A\r\n";
const unsigned char set_app_key[] = "mac set appkey 2E15AFC94A7EF69AE61A814737C38699\r\n";
const unsigned char getVerCommand[14] = "sys get ver\r\n";


const unsigned char send_data_part_1 = "mac tx cnf 1 ";
const unsigned char send_data_EOL = " \r\n";
const unsigned char delimiter = "_";
//TODO choose datarate, is dependent of packet size
const unsigned char set_data_rate[] = "mac set dr x\r\n";

const unsigned char join_network[]= "mac join otaa\r\n";

void initLoRa(void){
    // Set the devEUI
    sendUARTMessage(set_dev_eui);
    delay_ms(800);
    // Set the appEUI
    sendUARTMessage(set_app_eui);
    delay_ms(800);
    // Set the appKey
    sendUARTMessage(set_app_key);
    delay_ms(3000);
    // Try to join the network with OTAA
    sendUARTMessage(join_network);
    delay_ms(3000);
}
void sendData(double capacitance, double ambient_temp, double pipe_temp, double inductance){
    unsigned char message[200];
    sprintf(message,"mac tx cnf 1 %x%s%x%s%x%s%x \r\n",
            capacitance, delimiter, ambient_temp, pipe_temp, inductance);
    sendUARTMessage(message);
    delay_ms(3000);
}
