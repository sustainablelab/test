/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                main.c

Copyright (c) 2012 by Radiator Labs. All rights reserved.
This program contains the confidential trade secret information of Radiator Labs.
Use, disclosure, or copying without written consent is strictly prohibited.

-------------------------------------------------------------------------------
*/
/*  Author, Date, and Version
Author: MJG
Date: 2012 Aug 08
Version: 0.1
    Design Template
Version:
*/
/*  To Do
    1. Check polarity of FAN_PWM: does a LOW turn the fan on or off?
    2. Create I2C and UART software modules based on
        (I2C)  Zambetti's Wire.cpp & twi.c and 
        (UART) Zambetti's HardwareSerial.cpp, also example on p. 183 of the datasheet.
        What frame format and baud rate does the XBee use?
    3. Create a boot loader (programmed over ISP) for future programming via XBee.
        See p. 277.
    4. Provide for the XBee Serial Firmware Update (see p. 139). This requires connection to:
            XBee DIN    (send bootloader commands)
            XBee DOUT   (read output from bootloader menu)
            XBee RTS    (init sequence)
            XBee DTR    (init sequence)
            XBee RESET  (init sequence)
        What should control these lines?
        To update the XBee Firmware from a PC running X-CTU,
        use the FT232 bridge module,
        Break the XBee pins out to a connector,
        and connect the FT232 bridge to the board.

*/
/*  About hardware module USART0 (move to UART software module later):
    UBRRn (USART Baud Rate Register n): clk_sys clocks a down-counter that is loaded with the value in UBRRn; when the down-counter reaches 0, a clock is generated = fosc/(UBRRn+1). This is called the a "baud rate generator clock output" and it is further divided by 2, 8, or 16, depending on the number of states used in the state machine for the receiver's clock and data recovery.
    Set the baud rate, enable Rx and Tx, and set the frame format. Data in or out  goes in register UDRn.
    
*/
/*  Core ATmega Definitions */
#include <avr/io.h>
#include <avr/interrupt.h>

/* Radiator Labs Software Modules */
#include "common_macros.h"
#include "pins.h"
#include "modules/fans/fans.h"
#include "modules/tsen/tsen.h" // "2-wire Serial Interface" module for I2C
#include "modules/xbee/xbee.h" // "USART0" module in "Normal asynchronous" for serial communication

/* Function Prototypes */
void setup(void);
void loop(void);
//---------------PROGRAM EXECUTION BEGINS HERE---------------------------------

int
main (void) // The program counter points here on power-up/reset.
{
    setup();    // Call setup() only once.
    for ( ; ; )  // Call loop() in an infinite loop.
    {
        loop();
    }
    return 0;
}

void
setup (void)
{
    // Disable all interrupts during setup.
    cli();
    cfg_fans();
    cfg_xbee();
    cfg_sensors();
    
    xb_tx('B');
    
#if 1 // UART_MCU_DTR
    XBEE_DTR_PIN_HIGH;
    CFG_XBEE_DTR_PIN_AS_INPUT;
#endif  // UART_MCU_DTR
#if 1   // XBEE_RST
    XBEE_RST_PIN_HIGH;
    CFG_XBEE_RST_PIN_AS_OUTPUT;
#endif  // XBEE_RST
        // cfg I/O: Hardware Testing
#if 1   // MCU_CLKO
    MCU_CLKO_PIN_HIGH;
    CFG_MCU_CLKO_PIN_AS_OUTPUT; // Output HIGH.
#endif
    // Enable all enabled interrupts.
    sei();
    
}

void
loop (void)
{
    // Echo incoming bytes.
    //---------------------
    // Wait to receive a byte.
    // Store the byte in 'msg.'
    uint8_t msg = xb_rx();
    // Transmit the byte back.
    xb_tx(msg);
    // Follow up with 'F.'
    xb_tx('F');
#if 0
    uint8_t msg = 0;
    msg = xb_rx();
#endif
#if 0
    // Turn the fan on/off with the XBee.
    fan1_speed(40);
    fan2_speed(40);
    switch(msg)
    {
        case 'A':
            fans_on();
            break;
            
        case 'a':
            fans_off();
            break;
        
        default:
            xb_tx('?');
            break;
    }
#endif
#if 0

    // Reading temperature
    uint16_t temp = 0;
    uint8_t tsens_addr_pcb = SLA_PCB;
    uint8_t tsens_addr_radi = SLA_RADI;
    uint8_t tsens_addr_room = SLA_ROOM;
    switch(msg)
    {
        case 'A':
            fan1_speed(40);
            fan2_speed(40);
            fans_on();
            break;
            
        case 'a':
            fans_off();
            break;
            
        case 'P':
            temp = get_temperature(tsens_addr_pcb);
            // Send two bytes.
            xb_tx_temperature(temp);
            break;
        case 'R':
            temp = get_temperature(tsens_addr_radi);
            // Send two bytes.
            xb_tx_temperature(temp);
            break;
        case 'r':
            temp = get_temperature(tsens_addr_room);
            // Send two bytes.
            xb_tx_temperature(temp);
            break;
        case 'M':
            xb_tx(0);   // ASCII NULL character
            xb_tx(49);  // ASCII 1
            xb_tx(65);  // ASCII A
            xb_tx(150); // ASCII u with triangle
            xb_tx(200); // ASCII weird double line corner shape
            xb_tx(255); // ASCII nbsp
            xb_tx(255); // ASCII nbsp
            xb_tx(CR);
            break;
        case 'N':
            xb_tx(FAN1_EN_VAL);
            xb_tx(FAN1_PWM_VAL);
            xb_tx(FAN2_EN_VAL);
            xb_tx(FAN2_PWM_VAL);
            temp = get_temperature(tsens_addr_radi);
            xb_tx_temperature(temp); // 2 bytes
            temp = get_temperature(tsens_addr_room);
            xb_tx_temperature(temp);
            temp = get_temperature(tsens_addr_pcb);
            xb_tx_temperature(temp);
            break;
            
        default:
            // Unrecognized message.
            xb_tx('1');
            break;
    }
#endif // DIA_TEST
#if 0
    if ( 'A' == msg)
    {
        fans_on();
    }
    if ( 'a' == msg)
    {
        fans_off();        
    }
    msg = 0;
#endif
}

