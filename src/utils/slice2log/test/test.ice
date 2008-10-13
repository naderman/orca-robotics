#ifndef TEST_DATING_ICE
#define TEST_DATING_ICE

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
};

sequence<MyStruct0> MyStruct0Seq;
dictionary<string,MyStruct0> MyStruct0Dict;

struct MyStruct1
{   
    MyStruct0 mStruct;
    string mString;
};

class MyClass0
{
    int mInt;
};

class MyClass1 extends MyClass0
{
    double mDouble;
};

interface MyInterface
{
    MyStruct0 getStruct();
//         throws DataNotExistException, HardwareFailedException;
    MyClass0 getClass();
};

}; // module

#endif
