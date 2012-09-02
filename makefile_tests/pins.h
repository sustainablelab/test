/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                pins.h

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
Revision: 2012 Aug 21
    Changed pins to match the routing on part# main_board.
*/

#ifndef _H_PINS
#define _H_PINS

/*  Pin names match the associated net names on the schematic.  */
#if 1   // pin names: Fans
#define FAN1_TACH       PD4
#define FAN1_PWM        PD3 // OC2B
#define FAN1_EN         PD2
#define FAN2_TACH       PD6
#define FAN2_PWM        PD5 // OC0B
#define FAN2_EN         PD7
//-----PORT and DDR-----
#define FAN_PORT        PORTD
#define FAN_DDR         DDRD
#endif  // pin names: Fans
#if 1   // pin names: Temperature Sensor I2C Communication
#define SDA             PC4
#define SCL             PC5
//-----PORT and DDR-----
#define I2C_PORT        PORTC
#define I2C_DDR         DDRC
#endif  // pin names: Temperature Sensor I2C Communication
#if 1   // pin names: Serial UART Communication
// Serial Data (part of the USART0 hardware module)
#define UART_MCU_RX         PD0 // <- XBee DOUT
#define UART_MCU_TX         PD1 // -> XBee DIN
//-----PORT and DDR-----
#define UART_DATA_PORT      PORTD
#define UART_DATA_DDR       DDRD
// Serial Flow Control (implemented in software)
#define XBEE_CTS            PC0 // Flow Control output from XBee, LOW-asserted
#define XBEE_RTS            PC1 // Flow Control input to XBee, LOW-asserted
//-----PORT and DDR-----
#define FLOW_CTRL_PORT      PORTC
#define FLOW_CTRL_DDR       DDRC
#endif  // pin names: Serial UART Communication
#if 1   // pin names: XBee Serial Firmware Updates
                            //--------------------------------------------------------------
                            /*  Invoke XBee Bootloader  (see "XBee Bootloader" on p. 139) */
                            //--------------------------------------------------------------
                            // 0.  Disable hardware flow control
                            //     (configure RTS and CTS as DIO on the XBee;
                            //      leave UART_MCU_RTS and UART_MCU_CTS as they are).
#define XBEE_DTR        PC2 // 1a. Pull DTR LOW.
                            // 1b. Pull RTS HIGH (UART_MCU_RTS).
                            // 2a. Send a serial break (?) to XBee DIN (UART_MCU_TX).
#define XBEE_RST        PC3 // 2b. Set RST LOW (reset the XBee).
                            //  3. During power-up/reset hold DTR LOW, DIN LOW, and RTS HIGH.
                            //  4. Terminate serial break; send carriage return at 115200bps.
                            //  5. XBee sends the Ember bootloader menu at 115200bps on DOUT.
                            //     (should receive "BL>" prompt)
                            //  6. Send bootloader commands at 115200bps.
                            //     See "Send Firmware Image" on p.140 of the XBee datasheet.
//-----PORT and DDR-----
#define XBEE_FWARE_PORT PORTC
#define XBEE_FWARE_DDR  DDRC
#endif  // pin names: XBee Serial Firmware Updates
#if 1   // pin names: Hardware Testing
#define MCU_CLKO        PB0 // outputs the MCU clock (F_CPU) (failed to break out)
//-----PORT and DDR-----
#define MCU_CLKO_PORT   PORTB
#define MCU_CLKO_DDR    DDRB
#endif  // pin names: Hardware Testing

/*  Configure I/O
!!!!TO DO: Move these to the relevant modules!!!!
*/
#if 1   // cfg I/O: Hardware Testing
// MCU_CLKO
#define MCU_CLKO_PIN_HIGH           SETBIT(MCU_CLKO_PORT,MCU_CLKO)
#define CFG_MCU_CLKO_PIN_AS_OUTPUT  SETBIT(MCU_CLKO_DDR,MCU_CLKO)

#endif  // cfg I/O: Hardware Testing


#endif  /* _H_PINS */

