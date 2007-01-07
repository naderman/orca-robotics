/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

//!
//! This file contains functions relating to the chi-square distribution.
//!
//! The functions in this file were adapted from Javascript functions 
//! found at http://www.fourmilab.ch/rpkp/experiments/analysis/chiCalc.html
//!
//! They were adapted by John Walker from  C implementations
//! written by Gary Perlman of Wang Institute, Tyngsboro, MA
//! 01879.  Both the original C code and the JavaScript edition
//! are in the public domain. 
//!
//! \author Ian Mahon
//! \date 20-05-05
//!

#ifndef ORCA_CHI_SQUARE_H
#define ORCA_CHI_SQUARE_H

namespace orcanavutil {

//!
//! Calculate the probability of a chi-square value.
//!
//! \param x   The chi-square value.
//! \param df  The degrees of freedom of the chi-square distribution.
//!
//! \return    The probability of the chi-square value.
//!
//!  Adapted from:
//!          Hill, I. D. and Pike, M. C.  Algorithm 299
//!          Collected Algorithms for the CACM 1967 p. 243
//!  Updated for rounding errors based on remark in
//!          ACM TOMS June 1985, page 185
//!
double pochisq( double x, int df );



//!
//! Calculate the critical chi-square value for a given probability.
//!
//! \param p   The probability of the area in the chi-square distribution
//!            to the right of the critical value.
//! \param df  The degrees of freedom of the chi-square distribution.
//!
//! \return    The critical value.
//!
double critchi( double p, int df );

}

#endif
