/*
 * rf_driver.c
 *
 *  Created on: Feb 26, 2018
 *      Author: lourw
 */

#include "rf_driver.h"


void rf_Init(void)
{

    // declare address of other transceiver
    uint8_t addr[5] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00 };

    // setup port interrupt
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN0);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN0,
                                 GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN0);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN0);
    MAP_Interrupt_enableInterrupt(INT_PORT5);

    /* Initial values for nRF24L01+ library config variables */
    rf_crc = RF24_EN_CRC | RF24_CRCO; // CRC enabled, 16-bit
    rf_addr_width      = 5;
    rf_speed_power     = RF24_SPEED_1MBPS | RF24_POWER_0DBM;
    rf_channel         = 120;

    msprf24_init();  // All RX pipes closed by default
    msprf24_set_pipe_packetsize(0, 32);
    msprf24_open_pipe(0, 1);
    msprf24_standby();

    w_tx_addr(addr);
    w_rx_addr(0, addr);  // Pipe 0 receives auto-ack's, autoacks are sent back to the TX addr so the PTX node
                         // needs to listen to the TX addr on pipe#0 to receive them.

}

void rf_Send(uint8_t len, uint8_t *buf)
{
//    uint8_t payload[16];

    w_tx_payload(len, buf);
    msprf24_activate_tx();
//    r_rx_payload(32, payload);

}
