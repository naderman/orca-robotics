/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_TEST_ICE
#define ORCA_TEST_ICE

#include <orca/containers.ice>

// example of including another orca Slice file with classes
// they are slightly different: polarfeature2d has derived types in the same file with
// the base, and rangescanner2d does not.
#include <orca/polarfeature2d.ice>
#include <orca/rangescanner2d.ice>
// should it be this? 
// #include <orcaiface/polarfeature2d.ice>

// example of including an Ice Slice file
// it will be ignored slice2util
#include <IceStorm/IceStorm.ice>

module test
{

enum MyEnum
{
    Low,
    Medium,
    High
};

struct MyStruct0
{
    bool mBool;
    byte mByte;
    short mShort;
    int mInt;
    long mLong;
    float mFloat;
    double mDouble;
    ["orca:deg"] double mDoubleDeg;
    string mString;
};

sequence<MyStruct0> MyStruct0Seq;
dictionary<string,MyStruct0> MyStruct0Dict;

// special class to test meta data
class MyClass9
{
    double mDouble;
    ["slice2orca:string:degrees"] double mDoubleDeg;
};

// the rest of the classes test inheritance
class MyClass0
{
    bool mBool;
};

class MyClass1 extends MyClass0
{
    int mInt;
};

class MyClass2 extends MyClass0
{
    long mLong;
};

struct MyStruct1
{   
    MyStruct0 mStruct;
    orca::FloatSeq mOrcaSeq;
    MyStruct0Seq mSeq;
    MyClass0 mClass;
};

class MyClass3 extends MyClass1
{
    double mDouble;
};

class MyClass4 extends orca::SinglePolarFeature2d
{
    int mInt;
};

class MyClass5 extends orca::RangeScanner2dData
{
    int mInt;
};

interface MyInterface0
{
    MyStruct0 getStruct();
//         throws DataNotExistException, HardwareFailedException;
    MyClass0 getClass();
};

interface MyInterface1 extends MyInterface0
{
    void set( MyInterface0* iface );

//     idempotent IceStorm::Topic* topic();
};

}; // module

#endif
