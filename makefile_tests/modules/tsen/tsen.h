/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                tsen.h

Copyright (c) 2012 by Radiator Labs. All rights reserved.
This program contains the confidential trade secret information of Radiator Labs.
Use, disclosure, or copying without written consent is strictly prohibited.

-------------------------------------------------------------------------------
*/
/*  Author, Date, and Version
Author: MJG
Date: 2012 Aug 26
Version: 0.1
    Design Template
Revision: 
    
*/

#ifndef _H_TSEN
#define _H_TSEN

/*  Public Function Prototypes
-------------------------------------------------------------------------------
*/
void cfg_sensors(void);
uint16_t get_temperature(uint8_t);

/*  Macros
-------------------------------------------------------------------------------
*/
#if 1   // I2C slave addresses
#define SLA_PCB     0x48
#define SLA_RADI    0x49
#define SLA_ROOM    0x4A
#endif
#if 1   // cfg I/O: Temperature Sensor I2C Communication
// SDA
#define SDA_PIN_HIGH                    SETBIT(I2C_PORT,SDA)
#define SDA_PIN_LOW                     CLRBIT(I2C_PORT,SDA)
#define CFG_SDA_PIN_AS_INPUT            CLRBIT(I2C_DDR,SDA)
#define CFG_SDA_PIN_AS_OUTPUT           SETBIT(I2C_DDR,SDA)
// SCL
#define SCL_PIN_HIGH                    SETBIT(I2C_PORT,SCL)
#define SCL_PIN_LOW                     CLRBIT(I2C_PORT,SCL)
#define CFG_SCL_PIN_AS_INPUT            CLRBIT(I2C_DDR,SCL)
#define CFG_SCL_PIN_AS_OUTPUT           SETBIT(I2C_DDR,SCL)
#endif  // cfg I/O: Temperature Sensor I2C Communication

// I2C status
#define START           0x08
// Tx:
#define MT_SLA_ACK      0x18
#define MT_SLA_NACK     0x20 // not used yet
#define MT_DATA_ACK     0x28
#define MT_DATA_NACK    0x30 // not used yet
// Rx: (note that the DATA_ACK/NACK is sent by the master)
#define MR_SLA_ACK      0x40
#define MR_SLA_NACK     0x48 // not used yet
#define MR_DATA_ACK     0x50
#define MR_DATA_NACK    0x58

#endif  /* _H_TSEN */

