/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                xbee.h

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

#ifndef _H_XBEE
#define _H_XBEE

/*  Public Function Prototypes
-------------------------------------------------------------------------------
*/
void cfg_xbee(void);
void xb_tx(uint8_t);
uint8_t xb_rx(void);
/*  Macros
-------------------------------------------------------------------------------
*/


#endif  /* _H_XBEE */
#if 1   // cfg I/O: Serial UART Communication

#define UART_MCU_RX_PIN_HIGH            SETBIT(UART_DATA_PORT,UART_MCU_RX)
#define UART_MCU_RX_PIN_AS_INPUT        CLRBIT(UART_DATA_DDR, UART_MCU_RX)

#define UART_MCU_TX_PIN_HIGH            SETBIT(UART_DATA_PORT,UART_MCU_TX)
#define UART_MCU_TX_PIN_AS_OUTPUT       SETBIT(UART_DATA_DDR, UART_MCU_TX)

#define XBEE_CTS_PIN_HIGH               SETBIT(FLOW_CTRL_PORT,XBEE_CTS)
#define XBEE_CTS_PIN_AS_INPUT           CLRBIT(FLOW_CTRL_DDR, XBEE_CTS)

#define XBEE_RTS_PIN_HIGH               SETBIT(FLOW_CTRL_PORT,XBEE_RTS)
#define XBEE_RTS_PIN_LOW                CLRBIT(FLOW_CTRL_PORT,XBEE_RTS)
#define XBEE_RTS_PIN_AS_OUTPUT          SETBIT(FLOW_CTRL_DDR, XBEE_RTS)

#endif  // cfg I/O: UART Serial Communication

#if 1   // cfg I/O: XBee Serial Firmware Updates

#define XBEE_DTR_PIN_HIGH           SETBIT(XBEE_FWARE_PORT,XBEE_DTR)
#define CFG_XBEE_DTR_PIN_AS_INPUT   CLRBIT(XBEE_FWARE_DDR, XBEE_DTR)

#define XBEE_RST_PIN_HIGH           SETBIT(XBEE_FWARE_PORT,XBEE_RST)
#define CFG_XBEE_RST_PIN_AS_OUTPUT  SETBIT(XBEE_FWARE_DDR, XBEE_RST)

#endif  // cfg I/O: XBee Serial Firmware Updates
