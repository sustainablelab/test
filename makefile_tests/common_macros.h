#ifndef _H_COMMON_MACROS
#define _H_COMMON_MACROS

// Define pre-processor flag values TRUE and FALSE.
#define _T 1 // TRUE
#define _F 0 // FALSE

/*  Processor Clock Frequency

    1.  If F_CPU is already defined by an ATmega header,
        the preprocessor issues a warning.
    
    2.  F_CPU depends on the CKDIV8 fuse.
        Set flag CKDIV8_FUSE_CHECKED accordingly.
*/
#ifndef F_CPU
#define CKDIV8_FUSE_CHECKED _F // Factory default.
#if CKDIV8_FUSE_CHECKED
#define F_CPU   1000000
#else
#define F_CPU   8000000
#endif  /* CKDIV8_FUSE_CHECKED */
#else
#warning F_CPU already defined.
#endif /* F_CPU */

/*  Bit-Operation Macros */
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (unsigned char)(1<<BIT))
#define CLRBIT(ADDRESS,BIT) (ADDRESS &= (unsigned char)~(1<<BIT))
#define	CHKBIT(ADDRESS,BIT) (ADDRESS & (unsigned char)(1<<BIT))

#endif  /* _H_COMMON_MACROS */

