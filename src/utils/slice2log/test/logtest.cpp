/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <orcaifacelog/test.h>
#include <iostream>
using namespace std;

int main( int argc, char **argv )
{    
    {
        test::MyEnum in, out;
        in = test::Low;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
//         cout << "MyEnum\n'" << "' (" << ss.str().size() << ")" << endl << endl;
        cout << "MyEnum\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        test::MyStruct0 in, out;
        in.mBool = 0;
        in.mByte = 1;
        in.mShort = 2;
        in.mInt = 3;
        in.mLong = 4;
        in.mFloat = 5.;
        in.mDouble = 6.;
        in.mString = "a sample";
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyStruct0\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        test::MyStruct0Seq in, out;
        test::MyStruct0 elem;
        elem.mBool = 0;
        elem.mByte = 1;
        elem.mShort = 2;
        elem.mInt = 3;
        elem.mLong = 4;
        elem.mFloat = 5.;
        elem.mDouble = 6.;
        elem.mString = "a sample";
        in.push_back( elem );
        in.push_back( elem );
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyStruct0Seq\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl;
    }
    {
        test::MyStruct0Dict in, out;
        test::MyStruct0 elem;
        elem.mBool = 0;
        elem.mByte = 1;
        elem.mShort = 2;
        elem.mInt = 3;
        elem.mLong = 4;
        elem.mFloat = 5.;
        elem.mDouble = 6.;
        elem.mString = "a sample";
        in["one"] = elem;
        in["two"] = elem;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyStruct0Dict\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in != out ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    {
        test::MyClass0Ptr in = new test::MyClass0;
        test::MyClass0Ptr out;
        in->mBool = 0;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyClass0Ptr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in->mBool!=out->mBool ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    {
        test::MyClass1Ptr in = new test::MyClass1;
        test::MyClass1Ptr out;
        in->mBool = 0;
        in->mInt = 3;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyClass1Ptr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in->mBool!=out->mBool || in->mInt!=out->mInt ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    {
        test::MyClass3Ptr in = new test::MyClass3;
        test::MyClass3Ptr out;
        in->mBool = 0;
        in->mInt = 3;
        in->mDouble = 6.;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyClass3Ptr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in->mBool!=out->mBool || in->mInt!=out->mInt || in->mDouble!=out->mDouble ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    // the tough one, generic pointer, derived object.
    {
        test::MyClass1Ptr in = new test::MyClass3;
        test::MyClass1Ptr out;
        in->mBool = 0;
        in->mInt = 3;
        test::MyClass3Ptr alias = test::MyClass3Ptr::dynamicCast( in );
        alias->mDouble = 6.;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyClass1Ptr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( !out ) {
            cout<<"failed! resultant Ptr is null"<<endl;
            return EXIT_FAILURE;
        }
        if ( in->mBool!=out->mBool || in->mInt!=out->mInt ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    return EXIT_SUCCESS;
}
