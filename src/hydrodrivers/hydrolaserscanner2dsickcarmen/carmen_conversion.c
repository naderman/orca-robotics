/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "carmen_conversion.h"

double 
carmen_get_time_ms(void)
{
  struct timeval tv;
  double t;

  if (gettimeofday(&tv, NULL) < 0) 
    fprintf(stderr,"carmen_get_time_ms encountered error in gettimeofday : %s\n",
		strerror(errno));
  t = tv.tv_sec + tv.tv_usec/1000000.0;

/*  printf("TRACE(carmen_conversion.c): tv.tv_sec: %d, tv.tv_usec: %d\n",(int) (tv.tv_sec), (int) (tv.tv_usec)); */

  return t;
}

