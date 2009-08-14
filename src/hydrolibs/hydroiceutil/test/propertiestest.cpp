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
    props.setProperty( "Int.Good", "256" );
    props.setProperty( "Int.Alpha", "two" );
    if ( props.size()!=5 ) {
        cout<<"failed: wrong size, expected=5, got="<<endl<<props.size()<<endl;
        return EXIT_FAILURE;
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
        ret = props.getPropertyAsDouble( "NotExist", retDouble );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with double: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getPropertyAsDouble( "Double.Alpha", retDouble );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\texisting key with alpha double: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
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
        ret = props.getPropertyAsInt( "NotExist", retInt );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with int: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        ret = props.getPropertyAsInt( "Int.Alpha", retInt );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\texisting key with alpha int: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
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
