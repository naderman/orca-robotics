/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAMISC_UNCOPYABLE_H
#define ORCAMISC_UNCOPYABLE_H

namespace orcamisc {

//!
//! @brief Handy way to avoid unintended copies.  Inherit from this and
//! the compiler will barf if you try to copy the derived class.
//!
//! @author Alex Brooks
//!
class Uncopyable
{
public:
    Uncopyable() {}
private:
    Uncopyable(const Uncopyable&);
    void operator=(const Uncopyable&);
};

}

#endif
