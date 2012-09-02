/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                xbee.c

Copyright (c) 2012 by Radiator Labs. All rights reserved.
This program contains the confidential trade secret information of Radiator Labs.
Use, disclosure, or copying without written consent is strictly prohibited.

-------------------------------------------------------------------------------
*/
/*  Author, Date, and Version
Author: MJG
Date: 2012 Aug 28
Version: 0.1
    Design Template
Revision: 
    
*/
/*  Description
-------------------------------------------------------------------------------
This is a software module for communicating with the XBee mounted on the
PCB main_board_v2 for the Radiator Labs pilot at 47 Claremont.
-------------------------------------------------------------------------------
*/
/*  Core ATmega Definitions */
#include <avr/io.h>

#include "xbee.h"
#include "../../common_macros.h"
#include "../../pins.h"


/*  Private Function Prototypes
-------------------------------------------------------------------------------
*/
static void cfg_io(void);
static void cfg_uart (void);

/*  Public Function Definitions -- place in setup()
-------------------------------------------------------------------------------
*/
void
cfg_xbee (void)
{
    cfg_io(); // Set up inputs/outputs.
    cfg_uart();
}

void
xb_tx (uint8_t msg)
{
    // Wait for the transmit buffer to be empty.
    while ( !CHKBIT(UCSR0A,UDRE0) )
    {
        // If UDRE0 == 1, the buffer is empty.
        // Do nothing until the buffer is empty.
    }
    // Put the character into the buffer.
    // This sends the data.
    UDR0 = msg;
}

uint8_t
xb_rx (void)
{
    // Wait for the receive buffer to receive something.
    while ( !CHKBIT(UCSR0A,RXC0) )
    {
        // If RXC0 == 1, data was received.
        // Do nothing until data is received.
    }
    // Return the received data.
    return UDR0;
}

/*  Public Function Definitions -- place in loop()
-------------------------------------------------------------------------------
*/

/*  Private Function Definitions
-------------------------------------------------------------------------------
*/
static void
cfg_io (void)
{
#if 1   // cfg I/O: Serial UART Communication

    /* The Rx/Tx lines idle high.
    */
    UART_MCU_RX_PIN_HIGH;
    UART_MCU_RX_PIN_AS_INPUT;

    UART_MCU_TX_PIN_HIGH;
    UART_MCU_TX_PIN_AS_OUTPUT;

    /*  Flow control:
        CTS is controlled by the XBee.
        RTS is controlled by the ATmega328.
        CTS is normally low (XBee is OK to receive).
        RTS if normally low (ATmega328 is OK to receive).
    */
    XBEE_CTS_PIN_HIGH;
    XBEE_CTS_PIN_AS_INPUT;

    XBEE_RTS_PIN_LOW;
    XBEE_RTS_PIN_AS_OUTPUT;
    
#endif  // cfg I/O: UART Serial Communication
#if 1   // cfg I/O: XBee Serial Firmware Updates

    XBEE_DTR_PIN_HIGH;
    CFG_XBEE_DTR_PIN_AS_INPUT;

    XBEE_RST_PIN_HIGH;
    CFG_XBEE_RST_PIN_AS_OUTPUT;

#endif  // cfg I/O: XBee Serial Firmware Updates
}

static void
cfg_uart (void)
{

    //-------------------------------------------------------------------------
    //  p.179
    //  Set the Baud rate (bps):
    //      BAUD = F_CPU/(16*(UBRR0 + 1))
    //      where F_CPU = 8MHz
    //-------------------------------------------------------------------------
    UBRR0 = 51; // BAUD = 9615 bps <- 0.2% error from 9600 bps.
    // TO DO: Check that WinAVR is handling the fact that UBBR0 is a 16-bit register.

    //-------------------------------------------------------------------------
    //  p.195
    //  Do not double the USART transmission speed:
    //      U2X0 = 0
    //-------------------------------------------------------------------------
    CLRBIT(UCSR0A,U2X0);
    
    //-------------------------------------------------------------------------
    //  p.197
    //  Use USART0 in Asynchronous mode:
    //      UMSEL01 = 0
    //      UMSEL00 = 0
    //-------------------------------------------------------------------------
    CLRBIT(UCSR0C,UMSEL01);
    CLRBIT(UCSR0C,UMSEL00);
    
    //  Set the serial frame format to 8-N-1
    //-------------------------------------------------------------------------
    //  Set the frame size to 8 data bits.
    //  p.198
    //      UCSZ02 = 0
    //      UCSZ01 = 1
    //      UCSZ00 = 1
    //-------------------------------------------------------------------------
    CLRBIT(UCSR0B,UCSZ02);
    SETBIT(UCSR0C,UCSZ01);
    SETBIT(UCSR0C,UCSZ00);
    //-------------------------------------------------------------------------
    //  Set mode to no parity.
    //  p.198
    //      UPM01 = 0
    //      UPM00 = 0
    //-------------------------------------------------------------------------
    CLRBIT(UCSR0C,UPM01);
    CLRBIT(UCSR0C,UPM00);
    //-------------------------------------------------------------------------
    //  Set mode for one stop bit.
    //  p.198
    //      USBS0 = 0
    //-------------------------------------------------------------------------
    CLRBIT(UCSR0C,USBS0);
    
    //-------------------------------------------------------------------------
    //  Enable the Rx/Tx pins.
    //  p.183
    //      RXEN0 = 1
    //      TXEN0 = 1
    //-------------------------------------------------------------------------
    SETBIT(UCSR0B,RXEN0);
    SETBIT(UCSR0B,TXEN0);
}

    /*  Transparent Mode Communication Test
        XB001_SH = 13A200
        XB001_SL = 40790629
        XB002_SH = 13A200
        XB002_SL = 40790605
        XB006_SH = 13A200
        XB006_SL = 40790620
        
        XB001_DH = 13A200
        XB001_DL = 40790620
        XB006_DH = 13A200
        XB006_DL = 40790629
        
    */
    /*  Transparent Mode Communication Test
        See XBee datasheet p.32:
        Send:
            +++
            (wait one second for OK)
            ATDT 1F<CR>
            where:
                AT is constant
                DT is a command
                1F is a parameter (optional)
                <CR> is carriag return
    */
    /*  XBee Default Out-Of-The-Box Settings
        Modem: XB24-ZB
        Function set: ZIGBEE ROUTER AT
        Version: 22A0
        Serial Interfacing:
            (3) BD - Baud Rate
            (0) NB - Parity
            (0) SB - Stop Bits
            (3) RO - Packetization Timeout
            (1) D7 - DIO7 Configuration
            (0) D6 - DIO6 Configuration
    */
    /*  Serial Settings
        See XBee datasheet p.133:
            BD == 3 -> 9600 bps
            NB == 0 -> No parity
            SB == 0 -> 1 stop bit
            Packetization Timeout is how long the XBee waits before forming
            an RF packet to send. Leave default for now.
            D7 == 1 -> CTS flow control
            D6 == 0 -> Disabled (use D6 = 1 for RTS flow control)
            
    */

