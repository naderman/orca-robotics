/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/proputils.h>
#include <hydroutil/mathdefs.h>
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
    props->setProperty( "Int.Good", "256" );
    props->setProperty( "Int.Alpha", "two" );
    props->setProperty( "StringSequence", "test:exam:check" );
    props->setProperty( "Frame2d.Good", "1.0 2.0 180.0" );
    props->setProperty( "Frame2d.Short", "1.0 2.0" );
    props->setProperty( "Frame2d.Alpha", "1.0 2.0 three" );
    props->setProperty( "Frame3d.Good", "1.0 2.0 3.0  4.0 5.0 6.0" );
    props->setProperty( "Frame3d.Short", "1.0 2.0 3.0  4.0 5.0" );
    props->setProperty( "Frame3d.Alpha", "1.0 2.0 three  4.0 5.0" );
    props->setProperty( "Size3d.Good", "1.0 2.0 3.0" );
    props->setProperty( "Size3d.Short", "1.0 2.0" );
    props->setProperty( "Size3d.Alpha", "1.0 2.0 three" );
    props->setProperty( "IntVector.Good", "1 2 3" );
    props->setProperty( "IntVector.Double", "1 2.0 3" );
    props->setProperty( "IntVector.Alpha", "1 two 3" );
    props->setProperty( "DoubleVector.Good", "1.0 2.0 3.0" );
    props->setProperty( "DoubleVector.Alpha", "1.0 two 3.0" );
    int ret;

    cout<<"testing getProperty() ... ";
    std::string retString;
    ret = orcaice::getProperty( props, "String", retString );
    if ( ret!=0 || retString!="test" ) {
        cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<retString<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getProperty( props, "NotExist", retString );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with string: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyWithDefault() ... ";
    retString = orcaice::getPropertyWithDefault( props, "String", "default" );
    if ( retString!="test" ) {
        cout<<"failed"<<endl<<"\texisting key with default string: ret="<<retString<<endl;
        return EXIT_FAILURE;
    }
    retString = orcaice::getPropertyWithDefault( props, "NotExist", "default" );
    if ( retString!="default" ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default string: ret="<<retString<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDouble() ... ";
    double retDouble;
    ret = orcaice::getPropertyAsDouble( props, "Double.Good", retDouble );
    if ( ret!=0 || !NEAR(retDouble,99.9, 1E-6) ) {
        cout<<"failed"<<endl
        	<<"\texisting key with double: ret="<<ret<<"; value="<<retDouble<<endl
        	<<"return test="<<(int)(ret==0)<<" value test="<<(int)(retDouble==99.9)<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsDouble( props, "NotExist", retDouble );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with double: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsDouble( props, "Double.Alpha", retDouble );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha double: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsDoubleWithDefault() ... ";
    retDouble = orcaice::getPropertyAsDoubleWithDefault( props, "Double.Good", 1e6 );
    if ( !NEAR(retDouble,99.9, 1E-6) ) {
        cout<<"failed"<<endl<<"\texisting key with default double: ret="<<retDouble<<endl;
        return EXIT_FAILURE;
    }
    retDouble = orcaice::getPropertyAsDoubleWithDefault( props, "NotExist", 1e6 );
    if (  !NEAR(retDouble,1E6, 1E-6) ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default double: ret="<<retDouble<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsInt() ... ";
    int retInt;
    ret = orcaice::getPropertyAsInt( props, "Int.Good", retInt );
    if ( ret!=0 || retInt!=256 ) {
        cout<<"failed"<<endl<<"\texisting key with int: ret="<<ret<<"; value="<<retInt<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsInt( props, "NotExist", retInt );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with int: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsInt( props, "Int.Alpha", retInt );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha int: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsIntWithDefault() ... ";
    retInt = orcaice::getPropertyAsIntWithDefault( props, "Int.Good", 1024 );
    if ( retInt!=256 ) {
        cout<<"failed"<<endl<<"\texisting key with default int: ret="<<retInt<<endl;
        return EXIT_FAILURE;
    }
    retInt = orcaice::getPropertyAsIntWithDefault( props, "NotExist", 1024 );
    if ( retInt!=1024 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default int: ret="<<retInt<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;



    cout<<"testing getPropertyAsIntVector() ... ";
    std::vector<int> intVector;
    ret = orcaice::getPropertyAsIntVector( props, "IntVector.Good", intVector );
    if ( ret!=0 || intVector.size()!=3 || (intVector.at(0)+intVector.at(1)+intVector.at(2))!=6 ) {
        cout<<"failed"<<endl<<"\texisting key with good IntVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    intVector.clear();
    ret = orcaice::getPropertyAsIntVector( props, "NotExist", intVector );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with IntVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    intVector.clear();
    ret = orcaice::getPropertyAsIntVector( props, "IntVector.Double", intVector );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with double IntVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    intVector.clear();
    ret = orcaice::getPropertyAsIntVector( props, "IntVector.Alpha", intVector );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha IntVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsIntVectorWithDefault() ... ";
    intVector.clear();
    intVector.push_back( 1 );
    std::vector<int> retIntVector;
    retIntVector = orcaice::getPropertyAsIntVectorWithDefault( props, "IntVector.Good", intVector );
    if ( retIntVector.size()!=3 || (retIntVector.at(0)+retIntVector.at(1)+retIntVector.at(2))!=6 ) {
        cout<<"failed"<<endl<<"\texisting key with default good IntVector: ret(0)="<<retIntVector.at(0)<<endl;
        return EXIT_FAILURE;
    }
    retIntVector.clear();
    retIntVector = orcaice::getPropertyAsIntVectorWithDefault( props, "NotExist", intVector );
    if (  retIntVector.size()!=1 || retIntVector.at(0)!=1 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default IntVector: ret(0)="<<retIntVector.at(0)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDoubleVector() ... ";
    std::vector<double> doubleVector;
    ret = orcaice::getPropertyAsDoubleVector( props, "DoubleVector.Good", doubleVector );
    if ( ret!=0 || doubleVector.size()!=3 || !NEAR(doubleVector.at(0)+doubleVector.at(1)+doubleVector.at(2), 6.0, 1E-6) ) {
        cout<<"failed"<<endl<<"\texisting key with good DoubleVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    doubleVector.clear();
    ret = orcaice::getPropertyAsDoubleVector( props, "NotExist", doubleVector );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with DoubleVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    doubleVector.clear();
    ret = orcaice::getPropertyAsDoubleVector( props, "DoubleVector.Alpha", doubleVector );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha DoubleVector: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsDoubleVectorWithDefault() ... ";
    doubleVector.clear();
    doubleVector.push_back( 1.0 );
    std::vector<double> retDoubleVector;
    retDoubleVector = orcaice::getPropertyAsDoubleVectorWithDefault( props, "DoubleVector.Good", doubleVector );
    if ( retDoubleVector.size()!=3 || !NEAR(retDoubleVector.at(0)+retDoubleVector.at(1)+retDoubleVector.at(2), 6.0, 1E-6) ) {
        cout<<"failed"<<endl<<"\texisting key with default good DoubleVector: ret="<<retDoubleVector.at(0)<<endl;
        return EXIT_FAILURE;
    }
    retDoubleVector.clear();
    retDoubleVector = orcaice::getPropertyAsDoubleVectorWithDefault( props, "NotExist", doubleVector );
    if (  retDoubleVector.size()!=1 || !NEAR(retDoubleVector.at(0), 1.0, 1E-6) ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default DoubleVector: ret="<<retDoubleVector.at(0)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
