/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                tsen.c

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
/*  Description
-------------------------------------------------------------------------------
This is a software module for communicating with the two temperature sensors that connect to
PCB main_board_v2 and the one temperature sensor mounted on main_board_v2.
-------------------------------------------------------------------------------
*/
/*  Core ATmega Definitions */
#include <avr/io.h>

#include "tsen.h"
#include "../../common_macros.h"
#include "../../pins.h"

/* TMP102 I2C ADDRESS SCHEME
    GND:    0100 1000 = 0X48    PCB
    V+:     0100 1001 = 0X49    RADIATOR
    SDA:    0100 1010 = 0X4A    ROOM
    SCL:    0100 1011 = 0x4B
    Shift the address to the left one bit.
    Write address: clear bit 0
    GND:    1001 0000 = 0X90    PCB
    V+:     1001 0010 = 0X92    RADIATOR
    SDA:    1001 0100 = 0X94    ROOM
    SCL:    1001 0110 = 0x96
    Read address: set bit 0
    GND:    1001 0001 = 0X91    PCB
    V+:     1001 0011 = 0X93    RADIATOR
    SDA:    1001 0101 = 0X95    ROOM
    SCL:    1001 0111 = 0x97
    
*/

/*  Private Function Prototypes
-------------------------------------------------------------------------------
*/
static void cfg_io(void);
static void cfg_i2c(void);
#if 0
static void i2c_start(void);
#endif
/*  Public Function Definitions
-------------------------------------------------------------------------------
*/
void
cfg_sensors (void)
{
    cfg_io();
    cfg_i2c();
}

#if 0
uint8_t // Return 0 if no error, 1 if error.
get_temperature (uint8_t i2c_addr, uint16_t *ptr_data_in)
{
    // Default flag value: no error.
    uint8_t err_status = 0;
    
    // Calculate the slave read address.
    uint8_t i2c_w_addr, i2c_r_addr;
    i2c_w_addr = i2c_addr << 1;
    i2c_r_addr = i2c_w_addr + 1;

    // Tx START
    i2c_start();
    
}
#endif
#if 1
uint16_t
get_temperature (uint8_t i2c_addr)
{
    uint8_t i2c_w_addr, i2c_r_addr;
    i2c_w_addr = i2c_addr << 1;
    i2c_r_addr = i2c_w_addr + 1;
    // Read two bytes over I2C.
    // The TMP102 points to its Temperature Register on power-up.
    // No need to write the pointer register.
    // Just do an I2C read of length two.

    uint16_t degC_12bit = 0;

    //-------------------------------------------------------------------------
    // Tx START
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT (clears INT flag).
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    //  Wait for INT flag to get set by hardware. Indicates START done.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }
    //  Check TWSR for successful START.
    if ((TWSR & 0xF8) != START)
    {
        ;// TO DO: Write an error routine.
    }    
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Tx SLA+R
    //-------------------------------------------------------------------------
    //  Load TWDR with SLA+R.
    TWDR = i2c_r_addr; // TO DO: replace with variable that holds slave address.

    //  Write to TWCR. Write includes setting bit TWINT.
    TWCR = (1<<TWINT)|(1<<TWEN);
    
    //  Wait for INT flag to get set by hardware.
    //  Indicates slave adress was transmitted and ACK/NACK received.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }
    //  Check TWSR for successful packet and ACK.
    if ((TWSR & 0xF8) != MR_SLA_ACK)
    {
        ;// TO DO: Write an error routine.
    }

    //-------------------------------------------------------------------------
    // Rx DATA Byte 1
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT.
    //  Set TWEA to send an ACK.
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    
    //  Wait for INT flag to get set by hardware.
    //  Indicates data was received and ACK returned.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }

    //  Check TWSR for successful packet and ACK.
    if ((TWSR & 0xF8) != MR_DATA_ACK)
    {
        ;// TO DO: Write an error routine.
    }
    // Status == MR_DATA_ACK, so read the data.
    else
    {
        // Grab the received byte.
        uint8_t byte_in;
        byte_in = TWDR;
        // Store the byte in the MSB position of a 16-bit word.
        degC_12bit = byte_in;
        degC_12bit = degC_12bit << 8;
    }
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Rx DATA Byte 2
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT.
    //  Clear TWEA to send a NACK.
    TWCR = (1<<TWINT)|(1<<TWEN)|(0<<TWEA);
    
    //  Wait for INT flag to get set by hardware.
    //  Indicates data was received and ACK returned.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }

    //  Check TWSR for successful packet and NACK.
    if ((TWSR & 0xF8) != MR_DATA_NACK)
    {
        ;// TO DO: Write an error routine.
    }
    // Status == MR_DATA_NACK, so read the data.
    else
    {
        // Grab the received byte.
        uint8_t byte_in;
        byte_in = TWDR;
        // Store the byte in the LSB position.
        degC_12bit |= byte_in;
    }
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Tx STOP
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT (clears INT flag).
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
    
    //  The INT flag does not get set by hardware. Nothing to check.
    //-------------------------------------------------------------------------

    return degC_12bit;
}
#endif
/*  Private Function Definitions
-------------------------------------------------------------------------------
*/
#if 0
static void
i2c_start (void)
{
    //-------------------------------------------------------------------------
    // Tx START
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT (clears INT flag).
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    //  Wait for INT flag to get set by hardware. Indicates START done.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }
    //  Check TWSR for successful START.
    if ((TWSR & 0xF8) != START)
    {
        ;// TO DO: Write an error routine.
    }    
}
#endif

static void
cfg_io (void)
{
    // External 10k resistors pull-up the I2C bus.
    // Configure SDA and SCL as inputs (until I2C is needed)
    // and disable the internal pull-ups.
    CFG_SCL_PIN_AS_INPUT;
    CFG_SDA_PIN_AS_INPUT;
    SCL_PIN_LOW; // (Driving an input pin low disables the pull-up.)
    SDA_PIN_LOW;

}

static void
cfg_i2c (void)
{
    //  ATmega328 is the I2C master. TMP102 is the I2C slave.
    //  The I2C implementation here does not handle more than one master.
    
    //  Atmel calls I2C "TWI" or "2-wire Serial Interface."
    //  See application note AVR315.
    /*  See electrical characteristics on p. 321.
2-wire Serial Bus Requirements:
    F_CPU > 250kHz AND F_CPU > 16*F_I2C
        F_I2C is the frequency of the clock signal on SCL.
        FOR F_CPU = 1MHz, max(F_I2C) = 62.5kHz.
        FOR F_CPU = 8MHz, max(F_I2C) = 500kHz.
    min(Rp) = (Vcc-0.4V)/3mA
        For Vcc = 3.3V, strongest pull-up is 1k.
    max(Rp) = 1000ns/Cb
        For Rp = 10k, max(Cb) = 100pF.
        Cb := capacitance of one bus line.        
    */

    /*  I2C Description:
During a transmission, SDA can only change while SCL is low.
SDA must be stable when SCL is high.
START is defined as SDA falling while SCL is high.
STOP is defined as SDA rising while SCL is high.

Only the master can control SCL.
Slaves control SDA when sending a data byte or an ACK bit.

Packets are sent MSBit first.
All packets are 9 bits long.
An address packet is 7 address bits, a R/W bit, and an ACK bit.
R/W bit high -> read, low -> write.
A data packet is 8 data bits and an ACK bit.
ACK -> slave pulls SDA low on the 9th SCL cycle.

The master device is the transmitter during writes,
and the receiver during reads.
From the ATmega328 point of view,
a typical write transmission goes:
START; SLA_W_ACK; DATA_Tx_ACK;...DATA_Tx_ACK; STOP
and a typical read transmission goes:
START; SLA_W_ACK; DATA_Tx_ACK;
REP_START; SLA_R_ACK; DATA_Rx_ACK;...DATA_Rx_NACK; STOP;


    */

    /*  Use F_I2C = 25kHz.
        The formula for F_I2C (SCL frequency) is from p.222:
        F_I2C = F_CPU/(16+2*TWBR*PRESCALER)
        F_CPU is 1MHz, so TWBR is 12
        TWBR    F_I2C
        2       50.0kHz
        3       45.5kHz
        4       41.7kHz
        5       38.5kHz
        6       35.7kHz
        7       33.3kHz
        8       31.3kHz
        9       29.4kHz    
        10      27.7kHz
        11      26.3kHz
        12      25.0kHz    
        13      23.8kHz
        14      22.7kHz    
        15      21.7kHz
        F_CPU is 8MHz, so TWBR is 152
    */
//  p.45: PRTWI: Power Reduction TWI
    CLRBIT(PRR,PRTWI);

    // I2C Registers:
    //  TWBR -> bit rate
    //  TWCR -> control
    //  TWSR -> status
    //  TWDR -> address/data shift register
    //  TWAR -> address
    //  p.222
    //  TWBR = ((F_CPU / F_I2C) - 16) / (2*N);
    TWBR = 152; // For F_CPU = 8MHz and N=1 -> F_I2C = 25kHz
    //  p.243
    //  Allowed N are 1,4,16,64. For prescaler (N) = 1:
    //      TWSR,TWPS1 = 0
    //      TWSR,TWPS0 = 0
    CLRBIT(TWSR,TWPS1);
    CLRBIT(TWSR,TWPS0);
}

static void
i2c_tx (uint8_t i2c_addr, uint8_t data) // TO DO: need to write at least two bytes
                    // first is the pointer register, second is the value.
{
    uint8_t i2c_w_addr;
    i2c_w_addr = i2c_addr << 1;

    //-------------------------------------------------------------------------
    // Tx START
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT (clears INT flag).
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    //  Wait for INT flag to get set by hardware. Indicates START done.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }
    //  Check TWSR for successful START.
    if ((TWSR & 0xF8) != START)
    {
        ;// TO DO: Write an error routine.
    }    
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Tx SLA+W
    //-------------------------------------------------------------------------
    //  Load TWDR with SLA+W.
    TWDR = i2c_w_addr; // TO DO: replace with variable that holds slave address.

    //  Write to TWCR. Write includes setting bit TWINT.
    TWCR = (1<<TWINT)|(1<<TWEN);
    
    //  Wait for INT flag to get set by hardware.
    //  Indicates slave adress was transmitted and ACK/NACK received.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }
    //  Check TWSR for successful packet and ACK.
    if ((TWSR & 0xF8) != MT_SLA_ACK)
    {
        ;// TO DO: Write an error routine.
    }    

    //-------------------------------------------------------------------------
    // Tx DATA
    //-------------------------------------------------------------------------
    //  Load TWDR with DATA.
    TWDR = data;

    //  Write to TWCR. Write includes setting bit TWINT.
    TWCR = (1<<TWINT)|(1<<TWEN);    
    
    //  Wait for INT flag to get set by hardware.
    //  Indicates data was transmitted and ACK/NACK received.
    while (!(CHKBIT(TWCR,TWINT)))
    {
        ;// Do nothing until TWINT flag is set.
    }

    //  Check TWSR for successful packet and ACK.
    if ((TWSR & 0xF8) != MT_DATA_ACK)
    {
        ;// TO DO: Write an error routine.
    }    
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Tx STOP
    //-------------------------------------------------------------------------
    //  Write to TWCR. Write includes setting bit TWINT (clears INT flag).
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
    
    //  The INT flag does not get set by hardware. Nothing to check.
    //-------------------------------------------------------------------------
   
    // Transmit an ACK
    // whenever a data byte is received:
    SETBIT(TWCR,TWEA);
    
    //  p.243: TWSR: Use a bit-rate prescaler of 1.
    CLRBIT(TWSR,TWPS0);
    CLRBIT(TWSR,TWPS1);

}

