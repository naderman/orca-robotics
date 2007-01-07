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

    cout<<"testing toOrcaTimeOfDay() ... ";
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
    cout<<"ok"<<endl;
    
    cout<<"testing toOrcaDate() ... ";
    orca::Date d = orcaice::toOrcaDate( t );
//     cout<<endl<<orcaice::toString(d)<<endl;

    if ( d.year != 1970|| d.month != 1 || d.day != 1 ) {
        cout<<"failed"<<endl<<"in="<<t.toDateTime()<<"; out =UTC "<<orcaice::toString(d)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
