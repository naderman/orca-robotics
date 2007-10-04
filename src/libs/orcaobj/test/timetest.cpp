/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaobj/orcaobj.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    cout<<"testing toOrcaTime() and toIceTime() ... ";
    {
        IceUtil::Time iceTimeIn = IceUtil::Time::now();
        orca::Time orcaTime = orcaice::toOrcaTime( iceTimeIn );
        IceUtil::Time iceTimeOut = orcaice::toIceTime( orcaTime );

        if ( iceTimeIn != iceTimeOut ) {
            cout<<"failed"<<endl<<"in="<<iceTimeIn.toDateTime()<<"; out="<<iceTimeOut.toDateTime()<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"testing toOrcaTime() and timeAsDouble() ... ";
    {
        orca::Time orcaTimeIn = orcaice::getNow();
        orca::Time orcaTimeOut = orcaice::toOrcaTime( orcaice::timeAsDouble(orcaTimeIn) );

        // due to rounding a difference of 1 useconds between input and output is OK.
        // NEAR uses < and >, so the tolerance is given as 2.
        if ( orcaTimeIn.seconds != orcaTimeOut.seconds 
                || !NEAR(orcaTimeIn.useconds,orcaTimeOut.useconds,2) ) 
        {
            cout<<"failed"<<endl
                <<"in="<<orcaice::toString(orcaTimeIn)<<"; out="<<orcaice::toString(orcaTimeOut)<<endl
                <<"in="<<orcaTimeIn.seconds<<":"<<orcaTimeIn.useconds<<"; out="<<orcaTimeOut.seconds<<":"<<orcaTimeOut.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toOrcaTime() and timeAsDouble() with negative time ... ";
    {
        double timeIn = -10.123;
        orca::Time orcaTimeIn = orcaice::toOrcaTime( timeIn );
        double timeOut = orcaice::timeAsDouble( orcaice::toOrcaTime( timeIn) );

        // due to rounding a difference of 1 useconds between input and output is OK.
        // NEAR uses < and >, so the tolerance is given as 2.
        if ( !NEAR(timeIn,timeOut,1e-5) ) 
        {
            cout<<"failed; in="<<timeIn<<"; out="<<timeOut<<endl;
            cout<<"        in="<<orcaTimeIn.seconds<<":"<<orcaTimeIn.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toOrcaTimeOfDay() ... ";
    {
        // unix start of time
        IceUtil::Time t = IceUtil::Time::seconds(1*60*60 + 2*60 + 3)
                        + IceUtil::Time::milliSeconds(456);
        // this will print time in local timezone
    //     cout<<endl<<t.toDateTime()<<endl;
        orca::TimeOfDay tod = orcaice::toOrcaTimeOfDay( t );
    //     cout<<endl<<orcaice::toString(tod)<<endl;
    
        if ( tod.hours != 1 || tod.minutes != 2 || tod.seconds != 3.456 ) {
            cout<<"failed"<<endl<<"in="<<t.toDateTime()<<"; out =UTC "<<orcaice::toString(tod)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"testing toOrcaDate() ... ";
    {
        // unix start of time
        IceUtil::Time t = IceUtil::Time::seconds(1*60*60 + 2*60 + 3)
                        + IceUtil::Time::milliSeconds(456);
        orca::Date d = orcaice::toOrcaDate( t );
    //     cout<<endl<<orcaice::toString(d)<<endl;
    
        if ( d.year != 1970|| d.month != 1 || d.day != 1 ) {
            cout<<"failed"<<endl<<"in="<<t.toDateTime()<<"; out =UTC "<<orcaice::toString(d)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing timeDiffAsDouble() positive ... ";
    {
        orca::Time t1 = orcaice::toOrcaTime( 1000.700 );
        orca::Time t2 = orcaice::toOrcaTime( 1000.500 );
        double dt = orcaice::timeDiffAsDouble( t1,t2 );
        double expect = 0.200;

        if ( !NEAR(dt,expect,1e-5) ) {
            cout<<"failed"<<endl<<"dt="<<dt<<"; expect="<<expect<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing timeDiffAsDouble() negative ... ";
    {
        orca::Time t1 = orcaice::toOrcaTime( 1000.700 );
        orca::Time t2 = orcaice::toOrcaTime( 1000.900 );
        double dt = orcaice::timeDiffAsDouble( t1,t2 );
        double expect = -0.200;

        if ( !NEAR(dt,expect,1e-5) ) {
            cout<<"failed"<<endl<<"dt="<<dt<<"; expect="<<expect<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing timeDiff() positive ... ";
    {
        orca::Time t1 = orcaice::toOrcaTime( 1000.700 );
        orca::Time t2 = orcaice::toOrcaTime( 1000.500 );
        orca::Time dt = orcaice::timeDiff( t1,t2 );
        orca::Time expect = orcaice::toOrcaTime( 0.200 );

        if ( dt.seconds != expect.seconds 
                || !NEAR(dt.useconds,expect.useconds,5) ) {
            cout<<"failed"<<endl<<"dt="<<dt.seconds<<"|"<<dt.useconds
                <<"; expect="<<expect.seconds<<"|"<<expect.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing timeDiff() positive crossing seconds ... ";
    {
        orca::Time t1 = orcaice::toOrcaTime( 1001.700 );
        orca::Time t2 = orcaice::toOrcaTime( 1000.500 );
        orca::Time dt = orcaice::timeDiff( t1,t2 );
        orca::Time expect = orcaice::toOrcaTime( 1.200 );

        if ( dt.seconds != expect.seconds 
                || !NEAR(dt.useconds,expect.useconds,5) ) {
            cout<<"failed"<<endl<<"dt="<<dt.seconds<<"|"<<dt.useconds
                <<"; expect="<<expect.seconds<<"|"<<expect.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing timeDiff() negative ... ";
    {
        orca::Time t1 = orcaice::toOrcaTime( 1000.700 );
        orca::Time t2 = orcaice::toOrcaTime( 1000.900 );
        orca::Time dt = orcaice::timeDiff( t1,t2 );
        orca::Time expect = orcaice::toOrcaTime( -0.200 );

        if ( dt.seconds != expect.seconds 
                || !NEAR(dt.useconds,expect.useconds,5) ) {
            cout<<"failed"<<endl<<"dt="<<dt.seconds<<"|"<<dt.useconds
                <<"; expect="<<expect.seconds<<"|"<<expect.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing timeDiff() negative crossing seconds ... ";
    {
        orca::Time t1 = orcaice::toOrcaTime( 1000.700 );
        orca::Time t2 = orcaice::toOrcaTime( 1001.300 );
        orca::Time dt = orcaice::timeDiff( t1,t2 );
        orca::Time expect = orcaice::toOrcaTime( -0.600 );

        if ( dt.seconds != expect.seconds 
                || !NEAR(dt.useconds,expect.useconds,5) ) {
            cout<<"failed"<<endl<<"dt="<<dt.seconds<<"|"<<dt.useconds
                <<"; expect="<<expect.seconds<<"|"<<expect.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
