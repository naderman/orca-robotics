/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef __NDEFINES_H
#define __NDEFINES_H

namespace novatel{

///////////////////////////////////////////////////////////////////////
// Standard Types
///////////////////////////////////////////////////////////////////////
typedef  signed int     INT;     // Signed Machine word size   (Assume to be no larger than 16 bit)
typedef  unsigned int   UINT;    // Unsigned Machine word size (Assume to be no larger than 16 bit)

// Defined by microsoft
typedef  char           CHAR;    // Signed 1-byte
typedef  unsigned char  UCHAR;   // Unsigned 1-byte
typedef  signed short   SHORT;   // Signed 2-byte (Unsuported for the T-805)
typedef  unsigned short USHORT;  // Unsigned 2-byte (Unsuported for the T-805)
typedef  signed long    LONG;    // Signed 4-byte
typedef  unsigned long  ULONG;   // Unsigned 4-byte

typedef  float          FLOAT;   // Single precision - IEEE floating point 32 bits
typedef  double         DOUBLE;  // Double precision - IEEE floating point 64 bits
typedef  long double    LDOUBLE; // Extended double precision - 80 bits

typedef  int            BOOL;    // BOOL (TRUE or FALSE)
typedef  unsigned char  BOOLCHAR;// BOOL (TRUE or FALSE) 1-byte


///////////////////////////////////////////////////////////////////////
// Other Global Types
///////////////////////////////////////////////////////////////////////
typedef  DOUBLE         GPS_TIME;   // Time in seconds
typedef  LDOUBLE        BTIME;      // Big Time in bigger seconds

//////////////////////////
// BOOLEAN Values        
//////////////////////////
#define TRUE        (1)
#define FALSE       (0)
#define YES         (1)
#define NO          (0)
#define ON          (1)
#define OFF         (0)
#define GOOD        (1)
#define BAD         (0)

//////////////////////////
// NULLs        
//////////////////////////
#ifndef  NULL
#define  NULL  (0)    // Pointer
#endif
#ifndef  NUL
#define  NUL   (0)    // End of String
#endif

#define DBL_EQUAL(x,v) ((fabs((DOUBLE) (x) - (DOUBLE) (v) )) < DBL_EPSILON)

// Make character upper case
#define  TOUPPER(ch)    ((ch) >= 'a' && (ch) <= 'z' ? (ch)-'a'+'A' : (ch))

// Convert an ASCII hex character to a nibble
#define  CHARTOHEX(ch)  ((ch) >= 'A' ? ((ch) - 'A')+10 : (ch) - '0')

// 08/12/1999 [CEL]  I added a char in front of the definition for HEXTOCHAR as
//                   it was generating compiler errors in the form of
//                   significant figures may be lost due to conversion.
// Convert a nibble to a ASCII hex character
#define  HEXTOCHAR(hx)  char((hx) >= 0x0A ? (hx) + 'A' - 0x0A : (hx) + '0')

// Convert a nibble to a ASCII hex character
#define  ISHEX(hx)  (((hx) >= '0' && (hx) <= '9') || (TOUPPER(hx) >= 'A' && TOUPPER(hx) <= 'F'))

// Return the smallest of the two values
#define MIN(x,y)  ((x)<(y) ? (x) : (y))

// Return the largest of the two values
#define MAX(x,y)  ((x)<(y) ? (y) : (x))

// Return the square value
#define SQR(x) ((x)*(x))

// Compare DOUBLES
#define COMPARE_REAL(X, Y, D) ((fabs((X)-(Y)) < (D)) ? 1:0)

// Conversion factors
#define M2INM    ((double) (0.00053995680))  /* INM - International nautical mile */
#define M2FT     ((double) (1.0/0.3048))
#define M2MILE   ((double) (0.00062137119))
#define MBAR2KPA ((double) (1.0/10.0))
#define MBAR2INI ((double) (1.0/33.8639))
#define MBAR2MM  ((double) (1.0/1.33322))
#define MBAR2PSI ((double) (1.0/68.94757))

}
#endif
