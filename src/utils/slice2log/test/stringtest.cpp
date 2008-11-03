/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <orcaifacestring/test.h>
#include <iostream>
using namespace std;

int main( int argc, char **argv )
{    
    try {
    {
        test::MyEnum in;
        in = test::Low;
        cout<<ifacestring::toString( in )<<endl;
    }
    {
        test::MyStruct0 in;
        in.mBool = 0;
        in.mByte = 1;
        in.mShort = 2;
        in.mInt = 3;
        in.mLong = 4;
        in.mFloat = 5.;
        in.mDouble = 6.;
        in.mString = "a sample";
        cout<<endl<<"RECURSE=FULL"<<endl;
        cout<<ifacestring::toString( in )<<endl;
        cout<<endl<<"RECURSE=0"<<endl;
        cout<<ifacestring::toString( in, 0 )<<endl;
    }
    {
        test::MyStruct0Seq in;
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
        cout<<endl<<"EXPAND=FULL"<<endl;
        cout<<ifacestring::toString( in )<<endl;
        cout<<endl<<"EXPAND=0"<<endl;
        cout<<ifacestring::toString( in, 999, 0 )<<endl;
        cout<<endl<<"EXPAND=1"<<endl;
        cout<<ifacestring::toString( in, 999, 1 )<<endl;
    }
    {
        test::MyStruct0Dict in;
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
        cout<<endl<<"EXPAND=FULL"<<endl;
        cout<<ifacestring::toString( in )<<endl;
        cout<<endl<<"EXPAND=0"<<endl;
        cout<<ifacestring::toString( in, 999, 0 )<<endl;
        cout<<endl<<"EXPAND=1"<<endl;
        cout<<ifacestring::toString( in, 999, 1 )<<endl;
    }
    // null class
    cout<<"printing null class pointer...";
    {
        test::MyClass0Ptr in;
        try {
            cout<<ifacestring::toString( in )<<endl;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;
    // base class
    {
        test::MyClass0Ptr in = new test::MyClass0;
        in->mBool = 0;
        cout<<endl<<"BASE"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    // mid-derived class
    {
        test::MyClass1Ptr in = new test::MyClass1;
        in->mBool = 0;
        in->mInt = 3;
        cout<<endl<<"MID"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    // top-derived class
    {
        test::MyClass3Ptr in = new test::MyClass3;
        in->mBool = 0;
        in->mInt = 3;
        in->mDouble = 6.;
        cout<<endl<<"TOP"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    // the tough one, generic pointer, derived object.
    {
        test::MyClass1Ptr in = new test::MyClass3;
        in->mBool = 0;
        in->mInt = 3;
        test::MyClass3Ptr dIn = test::MyClass3Ptr::dynamicCast( in );
        dIn->mDouble = 6.;
        cout<<endl<<"TOP THRU MID POINTER"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    // class derived from another file/module (potentially from another distro)
    {
        test::MyClass4Ptr in = new test::MyClass4;
        in->type = 99;
        in->pFalsePositive=.05;
        in->pTruePositive=.95;
        in->mInt = 3;
        cout<<endl<<"TOP FROM ANOTHER FILE"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    // the super tough one, generic pointer from orca, derived object from satellite project.
    // subcase: the base class has no derivatives in its file
    {
        orca::RangeScanner2dDataPtr in = new test::MyClass5;
        in->timeStamp.seconds=11;
        in->timeStamp.useconds=12;
        in->minRange=0.1;
        in->maxRange=0.2;
        in->fieldOfView=0.3;
        in->startAngle=0.4;
        test::MyClass5Ptr dIn = test::MyClass5Ptr::dynamicCast( in );
        dIn->mInt = 3;
        cout<<endl<<"TOP THRU BASE POINTER FROM ANOTHER FILE (no derivatives)"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    // the super tough one, generic pointer from orca, derived object from satellite project.
    // subcase: the base class has some derivatives in its file, but not this one obviously.
    {
        orca::SinglePolarFeature2dPtr in = new test::MyClass4;
        in->type = 99;
        in->pFalsePositive=.05;
        in->pTruePositive=.95;
        test::MyClass4Ptr dIn = test::MyClass4Ptr::dynamicCast( in );
        dIn->mInt = 3;
        cout<<endl<<"TOP THRU BASE POINTER FROM ANOTHER FILE (some derivatives)"<<endl;
        cout<<ifacestring::toString( in )<<endl;
    }
    }
    catch ( const std::exception& e ) {
        cout<<endl<<"caught exception : "<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
