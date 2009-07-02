/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    try {
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
        in.mDoubleDeg = 3.14;
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
    // null class
    cout<<"logging null class pointer...";
    {
        test::MyClass0Ptr in;
        stringstream ss; 
        try {
            ifacelog::toLogStream( in, ss );
            cout<<"should've thrown"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;
    // base class
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
    // mid-derived class
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
    // top-derived class
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
        test::MyClass3Ptr dIn = test::MyClass3Ptr::dynamicCast( in );
        dIn->mDouble = 6.;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyClass1Ptr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( !out ) {
            cout<<"failed! resultant Ptr is null"<<endl;
            return EXIT_FAILURE;
        }
        test::MyClass3Ptr dOut = test::MyClass3Ptr::dynamicCast( out );
        if ( !dOut ) {
            cout<<"failed! resultant Ptr is not of the derived type"<<endl;
            return EXIT_FAILURE;
        }
        if ( in->mBool!=out->mBool || in->mInt!=out->mInt || dIn->mDouble != dOut->mDouble ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    // class derived from another file/module (potentially from another distro)
    {
        test::MyClass4Ptr in = new test::MyClass4;
        test::MyClass4Ptr out;
        in->type = 99;
        in->pFalsePositive=.05;
        in->pTruePositive=.95;
        in->mInt = 3;
        stringstream ss; 
        ifacelog::toLogStream( in, ss );
        cout << "MyClass4Ptr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
        ifacelog::fromLogStream( out, ss );
        if ( in->type!=out->type || in->mInt!=out->mInt ) {
            stringstream ss; 
            ifacelog::toLogStream( out, ss );
            cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
            return EXIT_FAILURE;
        }
        cout << "ok" << endl<<endl;
    }
    // the super tough one, generic pointer from orca, derived object from satellite project.
    // THIS DOES NOT WORK (asserts, but maybe should throw?)
    // (there're actually 2 subcases here, see stringtest.cpp)
//     {
//         orca::SinglePolarFeature2dPtr in = new test::MyClass4;
//         orca::SinglePolarFeature2dPtr out;
//         in->type = 99;
//         in->pFalsePositive=.05;
//         in->pTruePositive=.95;
//         test::MyClass4Ptr dIn = test::MyClass4Ptr::dynamicCast( in );
//         dIn->mInt = 3;
//         stringstream ss; 
//         ifacelog::toLogStream( in, ss );
//         cout << "SinglePolarFeature2dPtr\n'" << ss.str() << "' (" << ss.str().size() << ")" << endl;
//         ifacelog::fromLogStream( out, ss );
//         if ( !out ) {
//             cout<<"failed! resultant Ptr is null"<<endl;
//             return EXIT_FAILURE;
//         }
//         if ( in->type!=out->type ) {
//             stringstream ss; 
//             ifacelog::toLogStream( out, ss );
//             cout<<"failed! out: '"<<ss.str()<<"'"<<endl;
//             return EXIT_FAILURE;
//         }
//         cout << "ok" << endl<<endl;
//     }
    }
    catch ( const std::exception& e ) {
        cout<<endl<<"caught exception : "<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
