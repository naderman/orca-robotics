/*
  Copyright (c) 2005, 
	  Aaron Lefohn (lefohn@cs.ucdavis.edu)
  All rights reserved.

  This software is licensed under the BSD open-source license. See
  http://www.opensource.org/licenses/bsd-license.php for more detail.

  *************************************************************
  Redistribution and use in source and binary forms, with or 
  without modification, are permitted provided that the following 
  conditions are met:

  Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer. 

  Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution. 

  Neither the name of the University of Californa, Davis nor the names of 
  the contributors may be used to endorse or promote products derived 
  from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
  OF SUCH DAMAGE.
*/

/*
 * Modifications under above BSD license 
 * Copyright 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
 */

#ifndef HYDRO_GLU_GLERRORUTIL_H
#define HYDRO_GLU_GLERRORUTIL_H

// A simple OpenGL error checking routine. This compiles away
// to a no-op inline method if the NDEBUG preprocessor symbol
// is defined during compilation.
//
// - The first parameter (optional) is a string that can be 
//   used to indicate the location where the error check occurs.
//
// - The second parameter determines the destination of the error
//   message. It defaults to cerr, but could also be a file.

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

namespace hydroglu
{

#ifndef NDEBUG

/**
 * @brief OpenGL error checking utility.
 * 
 * This function will fetch, print out, and clear the opengl error messages.
 * If NDEBUG is defined it does nothing, other wise it has the above mentioned behavior.
 *
 * Typical Usage:
 * @verbatim
 * #include <hydroglu/hydroglu.h>
 * //... other code
 * checkErrorsGL( ERROR_INFO );
 * @endverbatim
 */
void checkErrorsGL( const char *file = NULL, const char *line = NULL, 
		                std::ostream& ostr = std::cerr );
#else

/**
 * @brief OpenGL error checking utility.
 * 
 * This function will fetch, print out, and clear the opengl error messages.
 * If NDEBUG is defined it does nothing, other wise it has the above mentioned behavior.
 *
 * Typical Usage:
 * @verbatim
 * #include <hydroglu/hydroglu.h>
 * //... other code
 * checkErrorsGL( ERROR_INFO );
 * @endverbatim
 */
inline void checkErrorsGL( const char *file = NULL, const char *line = NULL, 
			                     std::ostream& ostr = std::cerr ) 
{}
#endif 
}
#endif

