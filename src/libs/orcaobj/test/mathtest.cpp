#include <iostream>
#include <orcaobj/mathdefs.h>
#include <iomanip>

#ifdef NDEBUG
#undef NDEBUG
#endif

using namespace std;

#define EPS 1e-7;

template<typename T>
void
testNormalise()
{
    T angle;

    angle = -M_PI+EPS;
    NORMALISE_ANGLE(angle);
    assert( angle >= -M_PI && angle < M_PI );

    angle = -M_PI-EPS;
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
