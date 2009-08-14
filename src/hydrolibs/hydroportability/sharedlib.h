/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * HYDRO_LICENSE file included in this distribution.
 *
 */

#ifdef WIN32

#include <windows.h>

#define SOEXPORT __declspec(dllexport)
#define SOIMPORT __declspec(dllimport)

// Is dllmain really needed?

#else

// for non-windows systems we have to define this as an empty string
#define SOEXPORT

#endif

