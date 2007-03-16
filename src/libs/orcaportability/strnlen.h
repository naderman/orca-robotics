/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Christopher Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_PORTABILITY_STRNLEN_H
#define ORCA2_PORTABILITY_STRNLEN_H

// this defines HAVE_STRNLEN
// #include <config.h>

// eg. Solaris doesn't define strnlen in string.h, so define it here.
#if !HAVE_STRNLEN

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
