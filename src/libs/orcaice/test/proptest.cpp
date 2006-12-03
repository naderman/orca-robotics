/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/proputils.h>
#include <orcaobj/mathdefs.h>
#include <orcaobj/stringutils.h>

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
    if ( ret!=0 || retDouble!=99.9 ) {
        cout<<"failed"<<endl<<"\texisting key with double: ret="<<ret<<"; value="<<retDouble<<endl;
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
    if ( retDouble!=99.9 ) {
        cout<<"failed"<<endl<<"\texisting key with default double: ret="<<retDouble<<endl;
        return EXIT_FAILURE;
    }
    retDouble = orcaice::getPropertyAsDoubleWithDefault( props, "NotExist", 1e6 );
    if ( retDouble!=1e6 ) {
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


    cout<<"testing getPropertyAsStringSeq() ... ";
    Ice::StringSeq retStringSeq;
    ret = orcaice::getPropertyAsStringSeq( props, "StringSequence", retStringSeq );
    if ( ret!=0 || retStringSeq.size()!=3 || retStringSeq[0]!="test" ) {
        cout<<"failed"<<endl<<"\texisting key with string seq: ret="<<ret<<"; size="<<retStringSeq.size()<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsStringSeq( props, "NotExist", retStringSeq );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with string seq: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsStringSeqWithDefault() ... ";
    Ice::StringSeq defaultSeq;
    defaultSeq.push_back("standard");
    defaultSeq.push_back("normal");
    retStringSeq = orcaice::getPropertyAsStringSeqWithDefault( props, "StringSequence", defaultSeq );
    if ( retStringSeq.size()!=3 || retStringSeq[2]!="check" ) {
        cout<<"failed"<<endl<<"\texisting key with default string seq: ret="<<retStringSeq.size()<<endl;
        return EXIT_FAILURE;
    }
    retStringSeq = orcaice::getPropertyAsStringSeqWithDefault( props, "NotExist", defaultSeq );
    if ( retStringSeq.size()!=defaultSeq.size() || retStringSeq[1]!="normal" ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default string seq: ret="<<retStringSeq.size()<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toStringSeq() and toString() ... ";
    std::string str1 = "alpha:bravo:charlie:delta";
    Ice::StringSeq seq = orcaice::toStringSeq( str1 );
    std::string str2 = orcaice::toString( seq );
    if ( str1 != str2 ) {
        cout<<"failed"<<endl<<"in="<<str1<<"; out="<<str2<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsFrame2d() ... ";
    orca::Frame2d frame2d;
    ret = orcaice::getPropertyAsFrame2d( props, "Frame2d.Good", frame2d );
    if ( ret!=0 || (frame2d.p.x+frame2d.p.y)!=3.0
                // degrees are converted to radians and then they are hard to compare
                || !NEAR(frame2d.o,M_PI,0.001) ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame2d: ret="<<ret
            <<" angle :"<<frame2d.o<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsFrame2d( props, "Frame2d.Short", frame2d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with short Frame2d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsFrame2d( props, "Frame2d.Alpha", frame2d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha Frame2d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsFrame2d( props, "NotExist", frame2d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with Frame2d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsFrame3d() ... ";
    orca::Frame3d frame3d;
    ret = orcaice::getPropertyAsFrame3d( props, "Frame3d.Good", frame3d );
    if ( ret!=0 || frame3d.p.x!=1.0 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for x=1.0 value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || frame3d.p.y!=2.0 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for y=2.0 value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || frame3d.p.z!=3.0 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for z=3.0 value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    // degrees are converted to radians and then they are hard to compare
    if ( ret!=0 || (int)floor(frame3d.o.r/DEG2RAD_RATIO+1.0e-3)!=4 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for r=4.0 value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || (int)floor(frame3d.o.p/DEG2RAD_RATIO+1.0e-3)!=5 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for p=5.0 value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || (int)floor(frame3d.o.y/DEG2RAD_RATIO+1.0e-3)!=6 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for y=6.0 value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
//     if ( ret!=0 || (frame3d.p.x+frame3d.p.y+frame3d.p.z)!=6.0 
//     // degrees are converted to radians and then they are hard to compare
//                 || (int)floor((frame3d.o.p+frame3d.o.r+frame3d.o.y+1.0e-3)/DEG2RAD_RATIO)!=15 ) {
//         cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret
//             <<" sum of angles :"<<(int)floor((frame3d.o.p+frame3d.o.r+frame3d.o.y+1.0e-3)/DEG2RAD_RATIO)<<endl;
//         return EXIT_FAILURE;
//     }
    ret = orcaice::getPropertyAsFrame3d( props, "Frame3d.Short", frame3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with short Frame3d: ret="<<ret<<" value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsFrame3d( props, "Frame3d.Alpha", frame3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha Frame3d: ret="<<ret<<" value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsFrame3d( props, "NotExist", frame3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with Frame3d: ret="<<ret<<" value="<<orcaice::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsSize3d() ... ";
    orca::Size3d size3d;
    ret = orcaice::getPropertyAsSize3d( props, "Size3d.Good", size3d );
    if ( ret!=0 || (size3d.l+size3d.w+size3d.h)!=6.0 ) {
        cout<<"failed"<<endl<<"\texisting key with Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsSize3d( props, "Size3d.Short", size3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with short Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsSize3d( props, "Size3d.Alpha", size3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaice::getPropertyAsSize3d( props, "NotExist", size3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
