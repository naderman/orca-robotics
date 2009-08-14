/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * HYDRO_LICENSE file included in this distribution.
 *
 */

//some very common functions are not available in windows, define them here

#ifdef WIN32

//windows doesn't define M_PI... anywhere, what a useless math header
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//windows doesn't define isinf isnan or finite directly in vc 2005...
//its 2005 and ms has a million devs, you'd think... well nevermind
#include <float.h>

#ifndef isnan(n)
#define isnan(n) _isnan(n)
#endif

#ifndef isinf(n)
#define isinf(n) !_finite(n)
#endif

#ifndef finite(n)
#define finite(n) _finite(n)
#endif

#endif
