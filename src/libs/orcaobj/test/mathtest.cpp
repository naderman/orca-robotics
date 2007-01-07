/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaobj/mathdefs.h>
#include <iomanip>
#include <assert.h>

#ifdef NDEBUG
#undef NDEBUG
#endif

using namespace std;

#define EPS 1e-8;

template<typename T>
void
testNormalise()
{
    T angle;

    angle = -M_PI;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );

    angle = -M_PI+EPS;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );

    angle = -M_PI-EPS;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );

    angle = M_PI;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );

    angle = M_PI+EPS;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );

    angle = M_PI-EPS;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );
}

int main()
{
    testNormalise<float>();
    testNormalise<double>();

    cout << "Test PASSED" << endl;
}
