/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/proputils.h>
#include <gbxutilacfr/mathdefs.h>
// #include <orcaobj/stringutils.h>

using namespace std;

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    // Instantiate the global property set
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty( "String", "test" );
    props->setProperty( "Double.Good", "99.9" );
    props->setProperty( "Double.Alpha", "nine" );
    props->setProperty( "Double.Crap", "99.9 trailing crap" );
    props->setProperty( "Double.Comment", "99.9 # don't read this" );
    props->setProperty( "Int.Good", "256" );
    props->setProperty( "Int.Alpha", "two" );
    props->setProperty( "Int.Crap", "256 trailing crap" );
    props->setProperty( "Int.Comment", "256 # don't read this" );
    props->setProperty( "StringSequence", "test:exam:check" );
    props->setProperty( "IntVector.Good", "1 2 3" );
    props->setProperty( "IntVector.Alpha", "1 two 3" );
    props->setProperty( "DoubleVector.Good", "1.0 2.0 3.0" );
    props->setProperty( "DoubleVector.Alpha", "1.0 two 3.0" );
    // custom data types
    props->setProperty( "Frame2d.Good", "1.0 2.0 180.0" );
    props->setProperty( "Frame2d.Short", "1.0 2.0" );
    props->setProperty( "Frame2d.Alpha", "1.0 2.0 three" );
    props->setProperty( "Frame3d.Good", "1.0 2.0 3.0  4.0 5.0 6.0" );
    props->setProperty( "Frame3d.Short", "1.0 2.0 3.0  4.0 5.0" );
    props->setProperty( "Frame3d.Alpha", "1.0 2.0 three  4.0 5.0" );
    props->setProperty( "Size3d.Good", "1.0 2.0 3.0" );
    props->setProperty( "Size3d.Short", "1.0 2.0" );
    props->setProperty( "Size3d.Alpha", "1.0 2.0 three" );
    int ret;

    cout<<"testing getProperty() ... ";
    {
        std::string s;
        ret = orcaice::getProperty( props, "String", s );
        if ( ret!=0 || s!="test" ) {
            cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<s<<endl;
            return EXIT_FAILURE;
        }
        ret = orcaice::getProperty( props, "NotExist", s );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with string: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyWithDefault() ... ";
    {
        string s = orcaice::getPropertyWithDefault( props, "String", "default" );
        if ( s!="test" ) {
            cout<<"failed"<<endl<<"\texisting key with default string: ret="<<s<<endl;
            return EXIT_FAILURE;
        }
        s = orcaice::getPropertyWithDefault( props, "NotExist", "default" );
        if ( s!="default" ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default string: ret="<<s<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDouble() ... ";
    {
        double d;
        ret = orcaice::getPropertyAsDouble( props, "Double.Good", d );
        if ( ret!=0 || !NEAR(d,99.9, 1E-6) ) {
            cout<<"failed"<<endl
                <<"\texisting key with double: ret="<<ret<<"; value="<<d<<endl
                <<"return test="<<(int)(ret==0)<<" value test="<<(int)(d==99.9)<<endl;
            return EXIT_FAILURE;
        }
        ret = orcaice::getPropertyAsDouble( props, "NotExist", d );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with double: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        try {
            ret = orcaice::getPropertyAsDouble( props, "Double.Alpha", d );
            cout<<"failed"<<endl<<"\tshould not be able to parse alpha"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsDoubleWithDefault() ... ";
    {
        double d = orcaice::getPropertyAsDoubleWithDefault( props, "Double.Good", 1e6 );
        if ( !NEAR(d,99.9, 1E-6) ) {
            cout<<"failed"<<endl<<"\texisting key with default double: ret="<<d<<endl;
            return EXIT_FAILURE;
        }
        d = orcaice::getPropertyAsDoubleWithDefault( props, "NotExist", 1e6 );
        if (  !NEAR(d,1E6, 1E-6) ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default double: ret="<<d<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsInt() ... ";
    {
        int i;
        ret = orcaice::getPropertyAsInt( props, "Int.Good", i );
        if ( ret!=0 || i!=256 ) {
            cout<<"failed"<<endl<<"\texisting key with int: ret="<<ret<<"; value="<<i<<endl;
            return EXIT_FAILURE;
        }
        ret = orcaice::getPropertyAsInt( props, "NotExist", i );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with int: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        try {
            ret = orcaice::getPropertyAsInt( props, "Int.Alpha", i );
            cout<<"failed"<<endl<<"\tshould not be able to parse alpha"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
        try {
            ret = orcaice::getPropertyAsInt( props, "Double.Good", i );
            cout<<"failed"<<endl<<"\tshould not be able to parse double"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsIntWithDefault() ... ";
    {
        int i = orcaice::getPropertyAsIntWithDefault( props, "Int.Good", 1024 );
        if ( i!=256 ) {
            cout<<"failed"<<endl<<"\texisting key with default int: ret="<<i<<endl;
            return EXIT_FAILURE;
        }
        i = orcaice::getPropertyAsIntWithDefault( props, "NotExist", 1024 );
        if ( i!=1024 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default int: ret="<<i<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;



    cout<<"testing getPropertyAsIntVector() ... ";
    {
        std::vector<int> v;
        ret = orcaice::getPropertyAsIntVector( props, "IntVector.Good", v );
        if ( ret!=0 || v.size()!=3 || (v.at(0)+v.at(1)+v.at(2))!=6 ) {
            cout<<"failed"<<endl<<"\texisting key with good IntVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        v.clear();
        ret = orcaice::getPropertyAsIntVector( props, "NotExist", v );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with IntVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        try {
            orcaice::getPropertyAsIntVector( props, "DoubleVector.Good", v );
            cout<<"failed"<<endl<<"\tshould not be able to parse doubles"<<endl;
            return EXIT_FAILURE;
        }
        catch (...) {}
        try {
            orcaice::getPropertyAsIntVector( props, "IntVector.Alpha", v );
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
        std::vector<int> retIntVector;
        retIntVector = orcaice::getPropertyAsIntVectorWithDefault( props, "IntVector.Good", v );
        if ( retIntVector.size()!=3 || (retIntVector.at(0)+retIntVector.at(1)+retIntVector.at(2))!=6 ) {
            cout<<"failed"<<endl<<"\texisting key with default good IntVector: ret(0)="<<retIntVector.at(0)<<endl;
            return EXIT_FAILURE;
        }
        retIntVector.clear();
        retIntVector = orcaice::getPropertyAsIntVectorWithDefault( props, "NotExist", v );
        if (  retIntVector.size()!=1 || retIntVector.at(0)!=1 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default IntVector: ret(0)="<<retIntVector.at(0)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDoubleVector() ... ";
    {
        std::vector<double> v;
        ret = orcaice::getPropertyAsDoubleVector( props, "DoubleVector.Good", v );
        if ( ret!=0 || v.size()!=3 || !NEAR(v.at(0)+v.at(1)+v.at(2), 6.0, 1E-6) ) {
            cout<<"failed"<<endl<<"\texisting key with good DoubleVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        v.clear();
        ret = orcaice::getPropertyAsDoubleVector( props, "NotExist", v );
        if ( ret==0 ) {
            cout<<"failed"<<endl<<"\tnon-existing key with DoubleVector: ret="<<ret<<endl;
            return EXIT_FAILURE;
        }
        try {
            orcaice::getPropertyAsDoubleVector( props, "DoubleVector.Alpha", v );
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
        std::vector<double> retDoubleVector;
        retDoubleVector = orcaice::getPropertyAsDoubleVectorWithDefault( props, "DoubleVector.Good", v );
        if ( retDoubleVector.size()!=3 || !NEAR(retDoubleVector.at(0)+retDoubleVector.at(1)+retDoubleVector.at(2), 6.0, 1E-6) ) {
            cout<<"failed"<<endl<<"\texisting key with default good DoubleVector: ret="<<retDoubleVector.at(0)<<endl;
            return EXIT_FAILURE;
        }
        retDoubleVector.clear();
        retDoubleVector = orcaice::getPropertyAsDoubleVectorWithDefault( props, "NotExist", v );
        if (  retDoubleVector.size()!=1 || !NEAR(retDoubleVector.at(0), 1.0, 1E-6) ) {
            cout<<"failed"<<endl<<"\tnon-existing key with default DoubleVector: ret="<<retDoubleVector.at(0)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
