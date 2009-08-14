/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cstdlib>
#include <hydroutil/properties.h>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

int main(int argc, char * argv[])
{
    cout<<"testing constructor without props ... ";
    hydroutil::Properties::Config config( true );
    hydroutil::Properties props( config );
    cout<<"ok"<<endl;

    cout<<"testing setProperty() ... ";
    props.setProperty( "String", "test" );
    props.setProperty( "Double.Good", "99.9" );
    props.setProperty( "Double.Alpha", "nine" );
    props.setProperty( "Double.Crap", "99.9 trailing crap" );
    props.setProperty( "Double.Comment", "99.9 # don't read this" );
    props.setProperty( "Int.Good", "256" );
    props.setProperty( "Int.Alpha", "two" );
    props.setProperty( "Int.Crap", "256 trailing crap" );
    props.setProperty( "Int.Comment", "256 # don't read this" );
    props.setProperty( "IntVector.Good", "1 2 3" );
    props.setProperty( "IntVector.Alpha", "1 two 3" );
    props.setProperty( "DoubleVector.Good", "1.0 2.0 3.0" );
    props.setProperty( "DoubleVector.Alpha", "1.0 two 3.0" );
    if ( props.size()!=13 ) {
        cout<<"failed: wrong size, expected=13, got="<<endl<<props.size()<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing isDefined() ... ";
    {
        if ( !props.isDefined( "String" ) ) {
            cout<<"failed"<<endl<<"\tproperty key='String' should exist"<<endl;
            return EXIT_FAILURE;
        }
        if ( props.isDefined( "NotExist" ) ) {
            cout<<"failed"<<endl<<"\tproperty key='NotExist' should not exist"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing ensureDefined() ... ";
    {
        try {
            props.ensureDefined( "String" );
        }
        catch(...) {
            cout<<"failed"<<endl<<"\tproperty key='String' should exist"<<endl;
            return EXIT_FAILURE;
        }
        try {
            props.ensureDefined( "NotExist" );
            cout<<"failed"<<endl<<"\tproperty key='NotExist' should not exist"<<endl;
            return EXIT_FAILURE;    
        }
        catch(...)
        {}
    }
    cout<<"ok"<<endl;

    cout<<"testing getProperty() ... ";
    {
        string retString;
        int ret = props.getProperty( "String", retString );
        if ( ret!=0 || retString!="test" ) {
            cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<retString<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getProperty( "NotExist", retString );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with string: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyWithDefault() ... ";
    {
        string retString = props.getPropertyWithDefault( "String", "default" );
        if ( retString!="test" ) {
            cout<<"failed"<<endl<<"\texisting key with default string: ret="<<retString<<endl;
            return EXIT_FAILURE;
        }
        retString = props.getPropertyWithDefault( "NotExist", "default" );
        if ( retString!="default" ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default string: ret="<<retString<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDouble() ... ";
    {
        double retDouble;
        int ret = props.getPropertyAsDouble( "Double.Good", retDouble );
        if ( ret!=0 || !NEAR(retDouble,99.9, 1E-6) ) {
            cout<<"failed"<<endl
                    <<"\texisting key with double: ret="<<ret<<"; value="<<retDouble<<endl
                    <<"return test="<<(int)(ret==0)<<" value test="<<(int)(retDouble==99.9)<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getPropertyAsDouble( "Double.Comment", retDouble );
        if ( ret!=0 || !NEAR(retDouble,99.9, 1E-6) ) {
            cout<<"failed"<<endl
                    <<"\texisting key with double: ret="<<ret<<"; value="<<retDouble<<endl
                    <<"return test="<<(int)(ret==0)<<" value test="<<(int)(retDouble==99.9)<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getPropertyAsDouble( "NotExist", retDouble );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with double: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }

        try {
            props.getPropertyAsDouble( "Double.Alpha", retDouble );
            cout<<"failed"<<endl<<"\tshould not be able to parse alpha"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
        try {
            props.getPropertyAsDouble( "Double.Crap", retDouble );
            cout<<"failed"<<endl<<"\tshould not be able to parse trailing crap"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsDoubleWithDefault() ... ";
    {
        double retDouble = props.getPropertyAsDoubleWithDefault( "Double.Good", 1e6 );
        if ( !NEAR(retDouble,99.9, 1E-6) ) {
            cout<<"failed"<<endl<<"\texisting key with default double: ret="<<retDouble<<endl;
            return EXIT_FAILURE;
        }
        retDouble = props.getPropertyAsDoubleWithDefault( "NotExist", 1e6 );
        if (  !NEAR(retDouble,1E6, 1E-6) ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default double: ret="<<retDouble<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsInt() ... ";
    {
        int retInt;
        int ret = props.getPropertyAsInt( "Int.Good", retInt );
        if ( ret!=0 || retInt!=256 ) {
            cout<<"failed"<<endl<<"\texisting key with int: ret="<<ret<<"; value="<<retInt<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getPropertyAsInt( "Int.Comment", retInt );
        if ( ret!=0 || retInt!=256 ) {
            cout<<"failed"<<endl<<"\texisting key with int: ret="<<ret<<"; value="<<retInt<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getPropertyAsInt( "NotExist", retInt );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with int: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }

        try {
            props.getPropertyAsInt( "Int.Alpha", retInt );
            cout<<"failed"<<endl<<"\tshould not be able to parse alpha"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
        try {
            props.getPropertyAsInt( "Double.Good", retInt );
            cout<<"failed"<<endl<<"\tshould not be able to parse doubles"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
        try {
            props.getPropertyAsInt( "Int.Crap", retInt );
            cout<<"failed"<<endl<<"\tshould not be able to parse trailing crap"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsIntWithDefault() ... ";
    {
        int retInt = props.getPropertyAsIntWithDefault( "Int.Good", 1024 );
        if ( retInt!=256 ) {
            cout<<"failed"<<endl<<"\texisting key with default int: ret="<<retInt<<endl;
            return EXIT_FAILURE;
        }
        retInt = props.getPropertyAsIntWithDefault( "NotExist", 1024 );
        if ( retInt!=1024 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default int: ret="<<retInt<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsIntWithDefault() with a bool ... ";
    {
        bool retBool;
        retBool = props.getPropertyAsIntWithDefault( "Int.Good", 1024 );
        if ( retBool!=true ) {
            cout<<"failed"<<endl<<"\texisting key with bool: ret="<<(int)retBool<<endl;
            return EXIT_FAILURE;
        }
        retBool = props.getPropertyAsIntWithDefault( "NotExist", 0 );
        if ( retBool!=false ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default int: ret="<<(int)retBool<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsIntVector() ... ";
    {
        std::vector<int> v;
        int ret = props.getPropertyAsIntVector( "IntVector.Good", v );
        if ( ret!=0 || v.size()!=3 || (v.at(0)+v.at(1)+v.at(2))!=6 ) {
            cout<<"failed"<<endl<<"\texisting key with good IntVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        v.clear();
        ret = props.getPropertyAsIntVector( "NotExist", v );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with IntVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        v.clear();
        try {
            props.getPropertyAsIntVector( "DoubleVector.Good", v );
            cout<<"failed"<<endl<<"\tshould not be able to parse doubles"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
        v.clear();
        try {
            props.getPropertyAsIntVector( "IntVector.Alpha", v );
            cout<<"failed"<<endl<<"\tshould not be able to parse alpha"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsIntVectorWithDefault() ... ";
    {
        std::vector<int> v;
        v.push_back( 1 );
        std::vector<int> retV;
        retV = props.getPropertyAsIntVectorWithDefault( "IntVector.Good", v );
        if ( retV.size()!=3 || (retV.at(0)+retV.at(1)+retV.at(2))!=6 ) {
            cout<<"failed"<<endl<<"\texisting key with default good IntVector: ret(0)="<<retV.at(0)<<endl;
            return EXIT_FAILURE;
        }
        retV.clear();
        retV = props.getPropertyAsIntVectorWithDefault( "NotExist", v );
        if (  retV.size()!=1 || retV.at(0)!=1 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default IntVector: ret(0)="<<retV.at(0)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDoubleVector() ... ";
    {
        std::vector<double> v;
        int ret = props.getPropertyAsDoubleVector( "DoubleVector.Good", v );
        if ( ret!=0 || v.size()!=3 || !NEAR(v.at(0)+v.at(1)+v.at(2), 6.0, 1E-6) ) {
            cout<<"failed"<<endl<<"\texisting key with good DoubleVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        v.clear();
        ret = props.getPropertyAsDoubleVector( "NotExist", v );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with DoubleVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        v.clear();
        try {
            props.getPropertyAsDoubleVector( "DoubleVector.Alpha", v );
            cout<<"failed"<<endl<<"\tshould not be able to parse alpha"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDoubleVectorWithDefault() ... ";
    {
        std::vector<double> v;
        v.push_back( 1.0 );
        std::vector<double> retV;
        retV = props.getPropertyAsDoubleVectorWithDefault( "DoubleVector.Good", v );
        if ( retV.size()!=3 || !NEAR(retV.at(0)+retV.at(1)+retV.at(2), 6.0, 1E-6) ) {
            cout<<"failed"<<endl<<"\texisting key with default good DoubleVector: ret="<<retV.at(0)<<endl;
            return EXIT_FAILURE;
        }
        retV.clear();
        retV = props.getPropertyAsDoubleVectorWithDefault( "NotExist", v );
        if (  retV.size()!=1 || !NEAR(retV.at(0), 1.0, 1E-6) ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default DoubleVector: ret="<<retV.at(0)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    // now that the get functions have been tested
    cout<<"testing copy constructor ... ";
    {
        map<string,string> source;
        source["Common.Something"] = "some";
        source["Common.SomethingElse"] = "else";
        hydroutil::Properties props( source );
        if ( props.size()!=2 ) {
            cout<<"failed: wrong size, expected=2, got="<<endl<<props.size()<<endl;
            return EXIT_FAILURE;
        }
        string value;
        if ( props.getProperty( "Common.Something", value ) ) {
            cout<<"failed: 'Common.Something' should exist"<<endl;
            return EXIT_FAILURE;
        }
        if ( value != "some" ) {
            cout<<"failed: wrong value, expect='some' got="<<value<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;   

    cout<<"testing copy constructor with prefix removal ... ";
    {
        map<string,string> source;
        source["Common.Something"] = "some";
        source["Common.SomethingElse"] = "else";
        hydroutil::Properties props( source, "Common." );
        if ( props.size()!=2 ) {
            cout<<"failed: wrong size, expected=2, got="<<endl<<props.size()<<endl;
            return EXIT_FAILURE;
        }
        string value;
        if ( props.getProperty( "Something", value ) ) {
            cout<<"failed: 'Something' should exist"<<endl;
            cout<<props.toString()<<endl;
            return EXIT_FAILURE;
        }
        if ( value != "some" ) {
            cout<<"failed: wrong value, expect='some' got="<<value<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;   

    return EXIT_SUCCESS;
}
