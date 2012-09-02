/*  Copyright Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                fans.h

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

#ifndef _H_FANS
#define _H_FANS

/*  Public Function Prototypes
-------------------------------------------------------------------------------
*/
void cfg_fans(void);
void fans_off(void);
void fans_on(void);
void fan1_speed(uint8_t);
void fan2_speed(uint8_t);

/*  Macros
-------------------------------------------------------------------------------
*/
#if 1   // cfg I/O: Fans
// FAN1_TACH
#define FAN1_TACH_PIN_HIGH              SETBIT(FAN_PORT,FAN1_TACH)
#define CFG_FAN1_TACH_PIN_AS_INPUT      CLRBIT(FAN_DDR,FAN1_TACH)
// FAN1_PWM
#define FAN1_PWM_PIN_LOW                CLRBIT(FAN_PORT,FAN1_PWM)
#define CFG_FAN1_PWM_PIN_AS_OUTPUT      SETBIT(FAN_DDR,FAN1_PWM)
// FAN1_EN
#define FAN1_EN_PIN_LOW                 CLRBIT(FAN_PORT,FAN1_EN)
#define CFG_FAN1_EN_PIN_AS_OUTPUT       SETBIT(FAN_DDR,FAN1_EN)
#define FAN1_EN_PIN_HIGH                SETBIT(FAN_PORT,FAN1_EN)

// FAN2_TACH
#define FAN2_TACH_PIN_HIGH              SETBIT(FAN_PORT,FAN2_TACH)
#define CFG_FAN2_TACH_PIN_AS_INPUT      CLRBIT(FAN_DDR,FAN2_TACH)
// FAN2_PWM
#define FAN2_PWM_PIN_LOW                CLRBIT(FAN_PORT,FAN2_PWM)
#define CFG_FAN2_PWM_PIN_AS_OUTPUT      SETBIT(FAN_DDR,FAN2_PWM)
// FAN2_EN
#define FAN2_EN_PIN_LOW                 CLRBIT(FAN_PORT,FAN2_EN)
#define CFG_FAN2_EN_PIN_AS_OUTPUT       SETBIT(FAN_DDR,FAN2_EN)
#define FAN2_EN_PIN_HIGH                SETBIT(FAN_PORT,FAN2_EN)
#endif  // cfg I/O: Fans

#if 1   // cfg PWM
// p.101 Fast PWM mode with TOP = OCRnA -> WGM2:0 = 7
// p.102 Non-inverted PWM on pin OCnB -> COMnB1:0 = 2
// p.110 Prescaler choices: N=0,1,8,64,256,1024

/* F_PWM = F_CPU/(N*(OCR0A+1))
    (OCR0A + 1) is the number of Timer0 clock tics per PWM period.
    F_PWM should be ~25kHz for a computer fan, between 21kHz and 28kHz is OK:
        see: http://www.formfactors.org/developer/specs/4_Wire_PWM_Spec.pdf
    F_CPU := frequency of CLK_IO (depends on clock source and CKDIV8 fuse)
    N := Timer Prescaler -> bits CS02:0
*/
/* PWM output is set at BOTTOM and cleared on Compare Match. Therefore:
    For M% duty cycle -> OCR0B = (OCR0A+1)/(M/100)
    Let OCR0A = 199 to keep math easy -> OCR0B = 2*(duty cycle)
    ex.: For 50% duty cycle -> OCR0B = (OCR0A+1)/(50/100) = 200*(50/100) = 2*50
*/
/* F_PWM configurations I thought about:
    For F_CPU = 1MHz, N = 1, and OCR0A = 199 -> F_PWM =  5.00kHz <- too slow
    For F_CPU = 8MHz, N = 1, and OCR0A = 199 -> F_PWM = 40.00kHz <- too fast
    For F_CPU = 8MHz, N = 1, and OCR0A = 255 -> F_PWM = 31.25kHz <- too fast
    For F_CPU = 1MHz, N = 1, and OCR0A = 39 -> F_PWM =  25.00kHz <- resolution?
    F_CPU = 8MHz, N = 8, and OCR0A = 49 -> F_PWM =  20.00kHz <- too slow
    OCR0A = F_CPU/(N*F_PWM) - 1
*/
/*  Keep it simple: Use F_CPU = 8MHz for now.
    F_CPU = 8MHz, N = 8, and OCR0A = 39 -> F_PWM =  25.000kHz
    To stay in 21kHz to 28kHz range, the slowest (largest OCR0A) is:
    F_CPU = 8MHz, N = 8, and OCR0A = 45 -> F_PWM =  21.739kHz 
*/

// CLK_IO clock source: 8MHz external oscillator
//-----------------------------------------------------------------------------
#if 1   // FAN2 (Timer0)

        //----------------
        //  Fast PWM:
        //      WGM02 = 1
        //      WGM01 = 1
        //      WGM00 = 1
        //----------------
#define TMR0_USE_FAST_PWM                       \
    do                                          \
    {                                           \
        SETBIT(TCCR0B,WGM02);                   \
        SETBIT(TCCR0A,WGM01);                   \
        SETBIT(TCCR0A,WGM00);                   \
    }                                           \
    while(0)
    
        //----------------
        //  Normal port operation, OC0A disconnected:
        //      COM0A1 = 0
        //      COM0A0 = 0
        //----------------
#define TMR0_TOP_EQUALS_OC0A                    \
    do                                          \
    {                                           \
        CLRBIT(TCCR0A,COM0A1);                  \
        CLRBIT(TCCR0A,COM0A0);                  \
    }                                           \
    while(0)
    
        //----------------
        //  Prescaler N=8:
        //      CS02 = 0
        //      CS01 = 1
        //      CS00 = 0
        //----------------
#define TMR0_CLK_PRESCALER_EQUALS_8             \
    do                                          \
    {                                           \
        CLRBIT(TCCR0B,CS02);                    \
        SETBIT(TCCR0B,CS01);                    \
        CLRBIT(TCCR0B,CS00);                    \
    }                                           \
    while(0)

        //----------------
        //  TO DO: User control of PWM frequency.
        //  Hard-coded at 25kHz for now.
        //  F_CPU = 8MHz, N = 8
        //  F_PWM = 25kHz -> OCR0A = F_CPU/(N*F_PWM) - 1 = 39
        //----------------
#define SET_TMR0_PWM_FREQ_TO_25KHZ              \
    do                                          \
    {                                           \
        OCR0A = 39;                             \
        TMR0_USE_FAST_PWM;                      \
        TMR0_TOP_EQUALS_OC0A;                   \
        TMR0_CLK_PRESCALER_EQUALS_8;            \
    }                                           \
    while(0)

    /*  Example of setting the duty cycle:
        FAN2_DUTY_CYCLE(50);
        50% duty cycle -> OCR0B = (OCR0A+1)/2 = 100
        FAN2_DUTY_CYCLE(100);
        100% duty cycle -> OCR0B = 200, OCR0B > OCR0A -> OCR0B is never cleared
        FAN2_DUTY_CYCLE(0);
        0% duty cycle -> OCR0B = 0
        p.102: OCR0B outputs a tiny spike at each overflow of Timer0.
    */
#endif  // FAN2 (Timer0)
#if 1   // FAN1 (Timer2)
        //----------------
        //  Fast PWM:
        //      WGM22 = 1
        //      WGM21 = 1
        //      WGM20 = 1
        //----------------
#define TMR2_USE_FAST_PWM                       \
    do                                          \
    {                                           \
        SETBIT(TCCR2B,WGM22);                   \
        SETBIT(TCCR2A,WGM21);                   \
        SETBIT(TCCR2A,WGM20);                   \
    }                                           \
    while(0)

        //----------------
        //  Normal port operation, OC2A disconnected:
        //      COM2A1 = 0
        //      COM2A0 = 0
        //----------------
#define TMR2_TOP_EQUALS_OC2A                    \
    do                                          \
    {                                           \
        CLRBIT(TCCR2A,COM2A1);                  \
        CLRBIT(TCCR2A,COM2A0);                  \
    }                                           \
    while(0)
    
        //----------------
        //  Prescaler N=8:
        //      CS22 = 0
        //      CS21 = 1
        //      CS20 = 0
        //----------------
#define TMR2_CLK_PRESCALER_EQUALS_8             \
    do                                          \
    {                                           \
        CLRBIT(TCCR2B,CS22);                    \
        SETBIT(TCCR2B,CS21);                    \
        CLRBIT(TCCR2B,CS20);                    \
    }                                           \
    while(0)

        //----------------
        //  TO DO: User control of PWM frequency.
        //  Hard-coded at 5kHz for now.
        //  F_PWM = 5kHz -> OCR2A = F_CPU/(N*F_PWM) - 1 = 199
        //----------------
#define SET_TMR2_PWM_FREQ_TO_25KHZ              \
    do                                          \
    {                                           \
        OCR2A = 39;                             \
        TMR2_USE_FAST_PWM;                      \
        TMR2_TOP_EQUALS_OC2A;                   \
        TMR2_CLK_PRESCALER_EQUALS_8;            \
    }                                           \
    while(0)

#endif  // FAN1 (Timer2)
#endif  // cfg PWM

#if 0   // OBSOLETE: PWM duty cycle: Fans
#define FAN1_DUTY_CYCLE(PERCENT)   (OCR2B = 2*(unsigned char)(PERCENT))
#define FAN2_DUTY_CYCLE(PERCENT)   (OCR0B = 2*(unsigned char)(PERCENT))
#endif  // PWM duty cycle: Fans

#endif  /* _H_FANS */
