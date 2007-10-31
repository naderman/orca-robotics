/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_NOVATELSPAN_INSGPS_TIMEVAL_H
#define ORCA2_NOVATELSPAN_INSGPS_TIMEVAL_H

#include <sys/time.h>

namespace insgps{

struct timeval* fix_timeval(struct timeval* tv);

struct timeval operator-(const struct timeval& lhs, const timeval& rhs);
struct timeval operator+(const struct timeval& lhs, const struct timeval& rhs);


} //namespace

#endif
