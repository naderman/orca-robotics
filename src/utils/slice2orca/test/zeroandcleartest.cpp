/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <orcaifaceutil/test.h>
#include <iostream>
using namespace std;

int main( int argc, char **argv )
{    
    try {
    {
        test::MyEnum in,out;
        in = (test::MyEnum)99; // to shut up the warning
        ifaceutil::zeroAndClear( in );
        out = test::Low;
        if ( in != out ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        test::MyStruct0 in,out;
        in.mString = "a sample";
        ifaceutil::zeroAndClear( in );
        out.mBool = 0;
        out.mByte = 0;
        out.mShort = 0;
        out.mInt = 0;
        out.mLong = 0;
        out.mFloat = 0.;
        out.mDouble = 0.;
        if ( in != out ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        test::MyStruct0Seq in,out;
        test::MyStruct0 elem;
        in.push_back( elem );
        ifaceutil::zeroAndClear( in );
        if ( in != out ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    {
        test::MyStruct0Dict in,out;
        test::MyStruct0 elem;
        in["one"] = elem;
        ifaceutil::zeroAndClear( in );
        if ( in != out ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    // null class
    cout<<"null class pointer...";
    {
        test::MyClass0Ptr in;
        try {
            ifaceutil::zeroAndClear( in );
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;
    // base class
    {
        test::MyClass0Ptr in = new test::MyClass0;
        ifaceutil::zeroAndClear( in );
        if ( in->mBool ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    // mid-derived class
    {
        test::MyClass1Ptr in = new test::MyClass1;
        ifaceutil::zeroAndClear( in );
        if ( in->mBool || in->mInt ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    // top-derived class
    {
        test::MyClass3Ptr in = new test::MyClass3;
        ifaceutil::zeroAndClear( in );
        if ( in->mBool || in->mInt || in->mDouble) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    // the tough one, generic pointer, derived object.
    {
        test::MyClass1Ptr in = new test::MyClass3;
        ifaceutil::zeroAndClear( in );
        test::MyClass3Ptr dIn = test::MyClass3Ptr::dynamicCast( in );
        if ( dIn->mBool || dIn->mInt || dIn->mDouble) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    // class derived from another file/module (potentially from another distro)
    {
        test::MyClass4Ptr in = new test::MyClass4;
        ifaceutil::zeroAndClear( in );
        test::MyClass3Ptr dIn = test::MyClass3Ptr::dynamicCast( in );
        if ( in->type || in->pFalsePositive || in->pTruePositive || in->mInt ) {
            cout<<"failed! out: "<<ifaceutil::toString(in)<<endl;
            return EXIT_FAILURE;
        }
    }
    // the super tough one, generic pointer from orca, derived object from satellite project.
    // THIS DOES NOT WORK (asserts, but maybe should throw?)
    // subcase: the base class has no derivatives in its file
//     {
//         orca::RangeScanner2dDataPtr in = new test::MyClass5;
//         ifaceutil::zeroAndClear( in );
//     }
    // the super tough one, generic pointer from orca, derived object from satellite project.
    // subcase: the base class has some derivatives in its file, but not this one obviously.
//     {
//         orca::SinglePolarFeature2dPtr in = new test::MyClass4;
//         ifaceutil::zeroAndClear( in );
//     }

    }
    catch ( const std::exception& e ) {
        cout<<endl<<"caught exception : "<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
