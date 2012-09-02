/*  Copyleft Notice
-------------------------------------------------------------------------------                                                       

                                Nock_47_Claremont
                                bootldr.c

Created 2012 by Radiator Labs.
Based on work by D. Mellis and D. Cuartielles
-------------------------------------------------------------------------------
*/
/*  Author, Date, and Version
Author: MJG
Date: 2012 Aug 08
Version: 0.1
    This bootloader is code snippets from ATmegaBOOT_xx8.c
    which is from ATmegaBOOT.c.
    (below info copied from Adafruit's ATmegaBOOT_xx8.c)
    20070626: hacked for Arduino Diecimila (which auto-  
              resets when a USB connection is made to it)
              by D. Mellis                               
    20060802: hacked for Arduino by D. Cuartielles       
              based on a previous hack by D. Mellis      
              and D. Cuartielles                             
*/

/* Radiator Labs Software Modules */
#include "common_macros.h"
#include "pins.h"
#include "modules/xbee/xbee.h" // For configuring the "USART0" module.

#if 1   // From ATmegaBOOT_xx8.c
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

/* 20070707: hacked by David A. Mellis - after this many errors give up and launch application */
#define MAX_ERROR_COUNT 5

// MJG: Function Prototypes
char getch(void);
void nothing_response(void);
void putch(char);

/*  MJG: Point to program memory address 0x00.
    Specifically, point to whatever function
    happens to reside at address 0x00.
    To call the function at address 0x00,
    dereference app_start:
        (*app_start)();

    This is achieved by creating a function pointer called app_start.
    
    Bruce Eckel explains function pointers well (See TICPP, 2nd ed., vol. 1).
        (look right, nothing, look left and see *): app_start is a pointer
        (look right and see "()" ): to a function
        (void): that takes no arguments
        (look left): and returns void.

    Bruce Eckel also explains how to use a function pointer.
        "Once you define a pointer to a function,
        you must assign it to a function address before you can use it.
        Just as the address of an array arr[10] is produced by the
        array name without the brackets (arr), the address of a function
        func() is produced by the function name without its argument list."
        
        So as "arr" is the address of the first element in arr[10],
        "func" is the address of the first line in func().
        
        "To call the function, you dereference the pointer in the same way
        that you declared it."
        
        Let func() be a function. To define a function pointer:
            void (*fp) (void);
        To initialize it:
            fp = func;
        Alternatively, you can define and initialize:
            void (*fp) (void) = func;
        To call the function pointed to, dereference the pointer:
            (*fp)();
*/
void (*app_start)(void) = 0x0000;

#endif  // From ATmegaBOOT_xx8.c

// Wait 500k getch() while loop iterations before timing out.
#define GETCH_TIME_OUT 500000
// Signature bytes (see p.298 of the ATmega328 datasheet).
#define SIG_BYTE0   0x1E
#define SIG_BYTE1   0x95
#define SIG_BYTE2   0x0F
/*  32KB Flash composed of 16K 16-bit words:
    256 pages of 64 words per page.
    (see p.298 of the ATmega328 datasheet).
*/
#define PAGE_SIZE   0x40

// MCUSR (See p.54 of the ATmega328 datasheet).

int
main (void)
{
    /*  Configures the UART:
        baud rate (9600) and serial frame (8-N-1).
        
        Note: ATmegaBOOT_xx8.c uses BAUD = 19200.
        I used 9600 to stay consistent with the Baud rate
        default on the XBee. 9600 is also used in the main application.
    */
    cfg_xbee(); // Serial communication is via the XBee.
    
    /*  Run the bootloader in an infinite loop.
        This works as a FSM, but using if-else rather than switch-case.
    */
    for(;;)
    {
        //  Create a flag to keep track of whether byte 0 is ASCII '0'.
        //  The first byte must be '0' if this is a proper bootloader instruction.
        uint8_t f_byte0_is_0 = _F; // Initialize the flag to False.

        // Receive a byte over UART.
        uint8_t byte0 = getch();

        //  Begin FSM.

        //  State: receive byte0
        if ('0' == byte0)
        {
            //  Set the flag to True: byte0 is ASCII '0'.
            f_byte0_is_0 = _T;

            //  The next byte should be a space: ' '.
            nothing_response();
        }
        // If byte0 was not ASCII '0', something's wrong.
        else if (_F == f_byte0_is_0)
        {
            /*  Reset the microcontroller by
                Enabling the Watchdog System Reset,
                and letting the Watchdog timeout.
                See p.54 of the ATmega328 datasheet.
            */
              WDTCSR = _BV(WDE);
              while (1)
              {
                ; // Do nothing until the microcontroller resets (~16 ms).
              }
        }
        
        //  State: receive byte1
        /* Request programmer ID */
        else if('1' == ch)
        {
            if (getch() == ' ')
            {
                putch(0x14);
                putch('A');
                putch('V');
                putch('R');
                putch(' ');
                putch('I');
                putch('S');
                putch('P');
                putch(0x10);
            }
            else
            {
                if (++error_count == MAX_ERROR_COUNT)
                {
                    app_start();
                }
            }
        }

        
    }
    //  End of the bootloader.
}

/* getch() returns a char if available and exits the bootloader after a timeout.
*/
char getch (void)
{
    uint32_t count = 0;

    while(!(UCSR0A & _BV(RXC0)))
    {
        // This probably takes four clock cycles.
        count++;
        // Exit the bootloader if the timeout is exceeded.
        if (count > GETCH_TIME_OUT)
        {
            app_start();
        }
     }

     // Byte received before timeout; return it.
    return UDR0;
}

void nothing_response (void)
{
    if (' ' == getch())
    {
        putch(0x14);
        putch(0x10);
    }
    else
    {
        if (++error_count == MAX_ERROR_COUNT)
        {
            app_start();
        }
    }
}

// putch() Sends a byte as soon as it is able to.
void putch (char ch)
{
    while (!(UCSR0A & _BV(UDRE0)))
    {
        ; // Do nothing until....until what?
    }
    // Send the byte.
    UDR0 = ch;
}


#if 0   // From ATmegaBOOT_xx8.c
/* main program starts here */
int main(void)
{
// Up to line 357 in ATmegaBOOT_xx8.c

    /* forever loop */
    for (;;) {

	/* get character from UART */
	ch = getch();

	/* A bunch of if...else if... gives smaller code than switch...case ! */

	/* Hello is anyone home ? */ 
	if(ch=='0') {
	  firstchar = 1; // we got an appropriate bootloader instruction
	  nothing_response();
	} else if (firstchar == 0) {
	  // the first character we got is not '0', lets bail!
	  // autoreset via watchdog (sneaky!)
	  WDTCSR = _BV(WDE);
	  while (1); // 16 ms
	}

	/* Request programmer ID */
	/* Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry  */
	/* Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.  */
	else if(ch=='1') {
	    if (getch() == ' ') {
		putch(0x14);
		putch('A');
		putch('V');
		putch('R');
		putch(' ');
		putch('I');
		putch('S');
		putch('P');
		putch(0x10);
	    } else {
		if (++error_count == MAX_ERROR_COUNT)
		    app_start();
	    }
	}


	/* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response() */
	else if(ch=='@') {
	    ch2 = getch();
	    if (ch2>0x85) getch();
	    nothing_response();
	}


	/* AVR ISP/STK500 board requests */
	else if(ch=='A') {
	    ch2 = getch();
	    if(ch2==0x80) byte_response(HW_VER);		// Hardware version
	    else if(ch2==0x81) byte_response(SW_MAJOR);	// Software major version
	    else if(ch2==0x82) byte_response(SW_MINOR);	// Software minor version
	    else if(ch2==0x98) byte_response(0x03);		// Unknown but seems to be required by avr studio 3.56
	    else byte_response(0x00);				// Covers various unnecessary responses we don't care about
	}


	/* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
	else if(ch=='B') {
	    getNch(20);
	    nothing_response();
	}


	/* Parallel programming stuff  DON'T CARE  */
	else if(ch=='E') {
	    getNch(5);
	    nothing_response();
	}


	/* Enter programming mode  */
/* Erase device, don't care as we will erase one page at a time anyway.  */
	else if(ch=='P' || ch == 'R') {
	    nothing_response();
	}


	/* Leave programming mode  */
	else if(ch=='Q') {
	    nothing_response();
#ifdef WATCHDOG_MODS
	    // autoreset via watchdog (sneaky!)
	    WDTCSR = _BV(WDE);
	    while (1); // 16 ms
#endif
	}
	

	/* Set address, little endian. EEPROM in bytes, FLASH in words  */
	/* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
	/* This might explain why little endian was used here, big endian used everywhere else.  */
	else if(ch=='U') {
	    address.byte[0] = getch();
	    address.byte[1] = getch();
	    nothing_response();
	}


	/* Universal SPI programming command, disabled.  Would be used for fuses and lock bits.  */
	else if(ch=='V') {
	  if (getch() == 0x30) {
	    getch();
	    ch = getch();
	    getch();
	    if (ch == 0) {
	      byte_response(SIG1);
	    } else if (ch == 1) {
	      byte_response(SIG2); 
	    } else {
	      byte_response(SIG3);
	    } 
	  } else {
	    getNch(3);
	    byte_response(0x00);
	  }
	}


	/* Write memory, length is big endian and is in bytes  */
	else if(ch=='d') {
	    length.byte[1] = getch();
	    length.byte[0] = getch();
	    flags.eeprom = 0;
	    if (getch() == 'E') flags.eeprom = 1;
	    for (w=0;w<length.word;w++) {
		buff[w] = getch();	                        // Store data in buffer, can't keep up with serial data stream whilst programming pages
	    }
	    if (getch() == ' ') {
		if (flags.eeprom) {		                //Write to EEPROM one byte at a time
		  address.word <<= 1;
		  for(w=0;w<length.word;w++) {
#if defined(__AVR_ATmega168__)  || defined(__AVR_ATmega328P__)
		    while(EECR & (1<<EEPE));
		    EEAR = (uint16_t)(void *)address.word;
		    EEDR = buff[w];
		    EECR |= (1<<EEMPE);
		    EECR |= (1<<EEPE);
#else
		    eeprom_write_byte((void *)address.word,buff[w]);
#endif
		    address.word++;
		  }			
		}
		else {					        //Write to FLASH one page at a time
		    if (address.byte[1]>127) address_high = 0x01;	//Only possible with m128, m256 will need 3rd address byte. FIXME
		    else address_high = 0x00;
#ifdef __AVR_ATmega128__
		    RAMPZ = address_high;
#endif
		    address.word = address.word << 1;	        //address * 2 -> byte location
		    /* if ((length.byte[0] & 0x01) == 0x01) length.word++;	//Even up an odd number of bytes */
		    if ((length.byte[0] & 0x01)) length.word++;	//Even up an odd number of bytes
		    cli();					//Disable interrupts, just to be sure
			// HACKME: EEPE used to be EEWE
		    while(bit_is_set(EECR,EEPE));			//Wait for previous EEPROM writes to complete
		    asm volatile(
				 "clr	r17		\n\t"	//page_word_count
				 "lds	r30,address	\n\t"	//Address of FLASH location (in bytes)
				 "lds	r31,address+1	\n\t"
				 "ldi	r28,lo8(buff)	\n\t"	//Start of buffer array in RAM
				 "ldi	r29,hi8(buff)	\n\t"
				 "lds	r24,length	\n\t"	//Length of data to be written (in bytes)
				 "lds	r25,length+1	\n\t"
				 "length_loop:		\n\t"	//Main loop, repeat for number of words in block							 							 
				 "cpi	r17,0x00	\n\t"	//If page_word_count=0 then erase page
				 "brne	no_page_erase	\n\t"						 
				 "wait_spm1:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm1       \n\t"
				 "ldi	r16,0x03	\n\t"	//Erase page pointed to by Z
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"							 
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
#endif
				 "wait_spm2:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm2       \n\t"									 

				 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
				 "sts	%0,r16		\n\t"						 			 
				 "spm			\n\t"
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
#endif
				 "no_page_erase:		\n\t"							 
				 "ld	r0,Y+		\n\t"	//Write 2 bytes into page buffer
				 "ld	r1,Y+		\n\t"							 
							 
				 "wait_spm3:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm3       \n\t"
				 "ldi	r16,0x01	\n\t"	//Load r0,r1 into FLASH page buffer
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"
							 
				 "inc	r17		\n\t"	//page_word_count++
				 "cpi r17,%1	        \n\t"
				 "brlo	same_page	\n\t"	//Still same page in FLASH
				 "write_page:		\n\t"
				 "clr	r17		\n\t"	//New page, write current one first
				 "wait_spm4:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm4       \n\t"
#ifdef __AVR_ATmega163__
				 "andi	r30,0x80	\n\t"	// m163 requires Z6:Z1 to be zero during page write
#endif							 							 
				 "ldi	r16,0x05	\n\t"	//Write page pointed to by Z
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
				 "ori	r30,0x7E	\n\t"	// recover Z6:Z1 state after page write (had to be zero during write)
#endif
				 "wait_spm5:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm5       \n\t"									 
				 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
				 "sts	%0,r16		\n\t"						 			 
				 "spm			\n\t"					 		 
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
#endif
				 "same_page:		\n\t"							 
				 "adiw	r30,2		\n\t"	//Next word in FLASH
				 "sbiw	r24,2		\n\t"	//length-2
				 "breq	final_write	\n\t"	//Finished
				 "rjmp	length_loop	\n\t"
				 "final_write:		\n\t"
				 "cpi	r17,0		\n\t"
				 "breq	block_done	\n\t"
				 "adiw	r24,2		\n\t"	//length+2, fool above check on length after short page write
				 "rjmp	write_page	\n\t"
				 "block_done:		\n\t"
				 "clr	__zero_reg__	\n\t"	//restore zero register
#if defined(__AVR_ATmega168__)  || defined(__AVR_ATmega328P__)
				 : "=m" (SPMCSR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
#else
				 : "=m" (SPMCR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
#endif
				 );
		    /* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
		    /* exit the bootloader without a power cycle anyhow */
		}
		putch(0x14);
		putch(0x10);
	    } else {
		if (++error_count == MAX_ERROR_COUNT)
		    app_start();
	    }		
	}
    

        /* Read memory block mode, length is big endian.  */
        else if(ch=='t') {
	    length.byte[1] = getch();
	    length.byte[0] = getch();
#if defined __AVR_ATmega128__
	    if (address.word>0x7FFF) flags.rampz = 1;		// No go with m256, FIXME
	    else flags.rampz = 0;
#endif
	    address.word = address.word << 1;	        // address * 2 -> byte location
	    if (getch() == 'E') 
	      flags.eeprom = 1;
	    else
	      flags.eeprom = 0;
	    if (getch() == ' ') {		                // Command terminator
		putch(0x14);
		for (w=0;w < length.word;w++) {		        // Can handle odd and even lengths okay
		    if (flags.eeprom) {	                        // Byte access EEPROM read
#if defined(__AVR_ATmega168__)  || defined(__AVR_ATmega328P__)
			while(EECR & (1<<EEPE));
			EEAR = (uint16_t)(void *)address.word;
			EECR |= (1<<EERE);
			putch(EEDR);
#else
			putch(eeprom_read_byte((void *)address.word));
#endif
			address.word++;
		    }
		    else {

			if (!flags.rampz) putch(pgm_read_byte_near(address.word));
#if defined __AVR_ATmega128__
			else putch(pgm_read_byte_far(address.word + 0x10000));
			// Hmmmm, yuck  FIXME when m256 arrvies
#endif
			address.word++;
		    }
		}
		putch(0x10);
	    }
	}


        /* Get device signature bytes  */
        else if(ch=='u') {
	    if (getch() == ' ') {
		putch(0x14);
		putch(SIG1);
		putch(SIG2);
		putch(SIG3);
		putch(0x10);
	    } else {
		if (++error_count == MAX_ERROR_COUNT)
		    app_start();
	    }
	}


        /* Read oscillator calibration byte */
        else if(ch=='v') {
	    byte_response(0x00);
	}
// Up to line 733

}
#endif  // From ATmegaBOOT_xx8.c
