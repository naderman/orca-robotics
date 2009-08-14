/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Christopher Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_PORTABILITY_STRNLEN_H
#define HYDRO_PORTABILITY_STRNLEN_H

// this defines HAVE_STRNLEN
// #include <config.h>

// eg. Solaris doesn't define strnlen in string.h, so define it here.
#if !HAVE_STRNLEN

#include <cstring>

// inline the fucker to guard against multiple inclusion, without the
// hassle of a special lib.
inline size_t strnlen(const char *s, size_t maxlen) 
{
    char *p;
    if (s == NULL) {
        return maxlen;
    }
    p = (char *)memchr(s, 0, maxlen);
    if (p == NULL) {
        return maxlen;
    }
    return ((p - s) + 1);
}
#endif

#endif
