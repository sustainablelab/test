/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                fans.c

Copyright (c) 2012 by Radiator Labs. All rights reserved.
This program contains the confidential trade secret information of Radiator Labs.
Use, disclosure, or copying without written consent is strictly prohibited.

-------------------------------------------------------------------------------
*/
/*  Author, Date, and Version
Author: MJG
Date: 2012 Aug 25
Version: 0.1
    Design Template
Revision: 
    
*/
/*  Description
-------------------------------------------------------------------------------
This is a software module for controlling the two fans that connect to
PCB main_board_v2 for the Radiator Labs pilot at 47 Claremont.
-------------------------------------------------------------------------------
*/
/*  Core ATmega Definitions */
#include <avr/io.h>

#include "fans.h"
#include "../../common_macros.h"
#include "../../pins.h"


/*  Private Function Prototypes
-------------------------------------------------------------------------------
*/
static void cfg_io(void);
static void cfg_pwm(void);

/*  Public Function Definitions -- place in setup()
-------------------------------------------------------------------------------
*/
void
cfg_fans (void)
{
    cfg_io(); // Set up inputs/outputs.
    cfg_pwm(); // Set up the hardware timer modules for PWM.
}

/*  Public Function Definitions -- place in loop()
-------------------------------------------------------------------------------
*/
void
fans_off (void)
{
    //  FAN2
    //  Normal port operation, OC0B disconnected:
    //      COM0B1 = 0
    //      COM0B0 = 0
    CLRBIT(TCCR0A,COM0B1);
    CLRBIT(TCCR0A,COM0B0);

    FAN2_EN_PIN_LOW;

    //  FAN1
    //  Normal port operation, OC2B disconnected:
    //      COM2B1 = 0
    //      COM2B0 = 0
    CLRBIT(TCCR2A,COM2B1);
    CLRBIT(TCCR2A,COM2B0);

    FAN1_EN_PIN_LOW;
}

void
fans_on (void)
{
    //  FAN2
    //  Connect OC0B to its I/O pin.
    //  Clear 0C0B on Compare Match, set OC0B at BOTTOM (non-inverting PWM):
    //      COM0B1 = 1
    //      COM0B0 = 0
    SETBIT(TCCR0A,COM0B1);
    CLRBIT(TCCR0A,COM0B0);

    FAN2_EN_PIN_HIGH;

    //  FAN1
    //  Connect OC2B to its I/O pin.
    //  Clear 0C2B on Compare Match, set OC2B at BOTTOM (non-inverting PWM):
    //      COM2B1 = 1
    //      COM2B0 = 0
    SETBIT(TCCR2A,COM2B1);
    CLRBIT(TCCR2A,COM2B0);
    
    FAN1_EN_PIN_HIGH;

}

void
fan1_speed(uint8_t speed)
{
    //  Speed is an integer from 0 to 40.
    //  40 is always on.
    //  0 is just barely on.

    //  TO DO:
    //  Linearize the fan speeds,
    //  and make speed a number from 0 to ?.
    OCR2B = speed;
}

void
fan2_speed(uint8_t speed)
{
    OCR0B = speed;
}

/*  Private Function Definitions
-------------------------------------------------------------------------------
*/
static void
cfg_io (void)
{
    // cfg I/O: Fans
#if 1   // FAN1
    // FAN1_EN
    FAN1_EN_PIN_LOW;            
    CFG_FAN1_EN_PIN_AS_OUTPUT;  // Output LOW.
    // FAN1_TACH
    FAN1_TACH_PIN_HIGH;         // Enable internal pull-up.
    CFG_FAN1_TACH_PIN_AS_INPUT;
    // FAN1_PWM
    FAN1_PWM_PIN_LOW;           
    CFG_FAN1_PWM_PIN_AS_OUTPUT; // Output LOW.
#endif  // FAN1
#if 1   // FAN2
    // FAN2_EN
    FAN2_EN_PIN_LOW;            
    CFG_FAN2_EN_PIN_AS_OUTPUT;  // Output LOW.
    // FAN2_TACH
    FAN2_TACH_PIN_HIGH;         // Enable internal pull-up.
    CFG_FAN2_TACH_PIN_AS_INPUT;
    // FAN2_PWM
    FAN2_PWM_PIN_LOW;           
    CFG_FAN2_PWM_PIN_AS_OUTPUT; // Output LOW.
#endif  // FAN2    
}

static void
cfg_pwm (void)
{
    // Timer2 controls FAN1; Timer0 controls FAN2

    fans_off();
    //  FAN1
    SET_TMR0_PWM_FREQ_TO_25KHZ;
    //  FAN2
    SET_TMR2_PWM_FREQ_TO_25KHZ;
    
}
