#include <iostream>

using namespace std;

const char *USAGE_ARGS = "<hex> <hex>";

int16_t slotVal( int hi, int lo )
{
    int val = hi;
    val = val << 8;
    val |= lo;
    return val;
}

int main ( int argc, char **argv )
{
    if ( argc < 3 )
    {
        cout << "USAGE: " << argv[0] << " " << USAGE_ARGS << endl;
        exit(1);
    }

    int hi, lo;
    sscanf( argv[1], "%x", &hi );
    sscanf( argv[2], "%x", &lo );

    cout << slotVal(hi,lo) << endl;
}
