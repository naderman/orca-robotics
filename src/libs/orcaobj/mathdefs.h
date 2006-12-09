/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

/*! 
  @file 
  @brief Mathematical macros
  @ingroup orca_library_orcaobj 
 */

#ifndef ORCA2_MATH_DEFINITIONS_H
#define ORCA2_MATH_DEFINITIONS_H	

/*****************************************************************************
 * INCLUDE THE RELEVANT MATHS LIBRARIES
 *****************************************************************************/

// MSVC compiler requires this symbol before exposing the (apparently) 
// non-standard symbols M_PI, etc...
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif

#include <cmath>

/*****************************************************************************
 * CONSTANTS
 *****************************************************************************/
// M_PI is not defined after including cmath for the MS visual studio compiler?
#ifndef M_PI
//! Defines number Pi
#define M_PI 3.14159265358979323846
#endif

#ifndef NAN
//! Defines not-a-number
#define NAN (__builtin_nanf(""))
#endif

/*****************************************************************************
 * CONVERSION MACROS
 *****************************************************************************/
#ifndef DEG2RAD_RATIO
//! Convertion factor from degrees to radians.
#define DEG2RAD_RATIO	(M_PI/180.0)
#endif

//! Converts from degrees to radians.
#define DEG2RAD(deg)	((deg)*DEG2RAD_RATIO)
//! Converts from radians to degrees.
#define RAD2DEG(rad) 	((rad)/DEG2RAD_RATIO)

//! Normalises the angle [rad] to the range [-pi,pi)
//! Don't return the normalised angle, because it's easy to make the
//! mistake of doing: 'NORMALISE_ANGLE( myAngle )', ignoring the return value.
inline void NORMALISE_ANGLE( double &theta )
{
    double multiplier;

    if (theta >= -M_PI && theta < M_PI)
        return;

    multiplier = floor(theta / (2*M_PI));
    theta -= multiplier*2*M_PI;
    if (theta >= M_PI)
        theta -= 2*M_PI;
    else if (theta < -M_PI)
        theta += 2*M_PI;
}

//! Normalises the angle [rad] to the range [-pi,pi)
//! Don't return the normalised angle, because it's easy to make the
//! mistake of doing: 'NORMALISE_ANGLE( myAngle )', ignoring the return value.
inline void NORMALISE_ANGLE( float &theta )
{
    double thDouble = theta;
    NORMALISE_ANGLE( thDouble );
    theta = thDouble;
}

/*****************************************************************************
 * MATH MACROS
 *****************************************************************************/
// for compatability retain this old one
//#ifndef ABS
//#define ABS(x)           (std::abs(x))
//#endif
#ifndef MIN
//! Minimum of two numbers
#define MIN(x, y)        (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
//! Maximum of two numbers
#define MAX(x, y)        (((x) > (y)) ? (x) : (y))
#endif
#ifndef NORM2
//! Norm of a 2D vector
#define NORM2(x, y)      (sqrt((x)*(x)+(y)*(y)))
#endif
#ifndef NORM3
//! Norm of a 3D vector
#define NORM3(x, y, z)   (sqrt((x)*(x)+(y)*(y)+(z)*(z)))
#endif
#ifndef ROUND
//! Rounds double or float to the nearest integer.
#define ROUND(x)         ((int)(x+0.5))
#endif
#ifndef ROUND_TO
//! Rounds @c n to the nearest whole number of multiples of @c d.
//! For example, ROUND_TO(8,5) and ROUND_TO(12,5) will both result in 10.
#define ROUND_TO(n,d)    (d*rint(n/d))
#endif
//#ifndef SQR
//#define SQR(x)           ((x)*(x))
//#endif
#ifndef SIGN
//! Sign of a number.
#define SIGN(A)          ((A)<0?(-1):(1))
#endif
#ifndef HYPOTENEUSE
//! Finds hypoeneuse
#define HYPOTENEUSE(a,b) (sqrt(SQR(a)+SQR(b)))
#endif
#ifndef COS_LAW
//! Law of cosines.
#define COS_LAW(side1, side2, theta) \
        (sqrt(SQR(side1)+SQR(side2)-2.0*(side1)*(side2)*cos(theta)))
#endif
#ifndef INV_COS_LAW
//! Inverse law of cosines..
#define INV_COS_LAW(oppSide, side1, side2) \
        (acos((SQR(side1)+SQR(side2)-SQR(oppSide))/(2.0*(side1)*(side2))))
#endif

// isinf not defined on all systems (eg Solaris)
#if defined (__SVR4) && defined (__sun)
#define	isfinite(x) \
  __extension__ ({ __typeof (x) __x_f = (x); \
		   __builtin_expect(!isnan(__x_f - __x_f), 1); })

#define	isinf(x) \
  __extension__ ({ __typeof (x) __x_i = (x); \
		   __builtin_expect(!isnan(__x_i) && !isfinite(__x_i), 0); })
#endif


/*****************************************************************************
 * COMPARISON MACROS
 *****************************************************************************/
#ifndef NEAR
//! Check that two numbers are sufficiently close.
//!  Compares using less_than and greater_than.
#define NEAR(x,y,epsilon) (((x) > (y)-(epsilon)) && ((x) < (y)+(epsilon))) 
#endif
#ifndef CHECK_LIMITS
//! Check that a number lies within limits. 
//! Compares using less_than and greater_than.
#define CHECK_LIMITS(max_x, x, min_x) \
        ((x)>(max_x)?(max_x):((x)<(min_x)?(min_x):(x)))
#endif
     
#endif
