/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaobj/orcaobj.h>
#include <orcaiceutil/mathdefs.h>
#include <iostream>

using namespace std;

int main(int argc, char * argv[])
{
    cout<<"testing toComponentName() and toString(orca::FQComponentName) ... ";
    {
        std::string s1 = "platf/comp";
        orca::FQComponentName fqComp = orcaice::toComponentName( s1 );
        std::string s2 = orcaice::toString( fqComp );
        if ( s1 != s2 ) {
            cout<<"failed"<<endl<<"in="<<s1<<"; out="<<s2<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"testing toInterfaceName() and toString(orca::FQInterfaceName) ... ";
    std::string strIface1 = "iface@platf/comp";
    orca::FQInterfaceName fqIface = orcaice::toInterfaceName( strIface1 );
    std::string strIface2 = orcaice::toString( fqIface );
    if ( strIface1 != strIface2 ) {
        cout<<"failed"<<endl<<"in="<<strIface1<<"; out="<<strIface2<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toString(orca::FQTopicName) ... ";
    std::string strTopic1 = "iface/topic@platf/comp";
    // back-conversion function does not exist, create the right topic manually
    //orca::FQTopicName fqTopic = orcaice::stringToTopicName( strTopic1 );
    orca::FQTopicName fqTopic;
    fqTopic.platform = "platf";
    fqTopic.component = "comp";
    fqTopic.iface = "iface";
    fqTopic.topic = "topic";
    std::string strTopic2 = orcaice::toString( fqTopic );
    if ( strTopic1 != strTopic2 ) {
        cout<<"failed"<<endl<<"in="<<strTopic1<<"; out="<<strTopic2<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toCartesianPoint2d() and the inverse toString() ... ";
    {
        orca::CartesianPoint2d in, out;
        orcaice::setSane( in );
        std::string s = orcaice::toString( in );
        int ret = orcaice::toCartesianPoint2d( s, out );
        if ( ret || !NEAR(in.x,out.x,1E-6) || !NEAR(in.y,out.y,1E-6) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<orcaice::toString(in)<<endl
                <<"str="<<s<<endl
                <<"out="<<orcaice::toString(out)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toCartesianPoint() and the inverse toString() ... ";
    {
        orca::CartesianPoint in, out;
        orcaice::setSane( in );
        std::string s = orcaice::toString( in );
        int ret = orcaice::toCartesianPoint( s, out );
        if ( ret || !NEAR(in.x,out.x,1E-6) || !NEAR(in.y,out.y,1E-6) || !NEAR(in.z,out.z,1E-6) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<orcaice::toString(in)<<endl
                <<"str="<<s<<endl
                <<"out="<<orcaice::toString(out)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFrame2d() and the inverse toString() ... ";
    {
        orca::Frame2d in, out;
        orcaice::setSane( in );
        std::string s = orcaice::toString( in );
        int ret = orcaice::toFrame2d( s, out );
        // angled must be compared approximately
        if ( ret || !NEAR(in.p.x,out.p.x,1E-6) || !NEAR(in.p.y,out.p.y,1E-6) || !NEAR(in.o,out.o,1E-3) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<orcaice::toString(in)<<endl
                <<"str="<<s<<endl
                <<"out="<<orcaice::toString(out)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;
    
    cout<<"testing toFrame3d() and the inverse toString() ... ";
    {
        orca::Frame3d in, out;
        orcaice::setSane( in );
        std::string s = orcaice::toString( in );
        int ret = orcaice::toFrame3d( s, out );
        // angled must be compared approximately
        if ( ret || !NEAR(in.p.x,out.p.x,1E-6) || !NEAR(in.p.y,out.p.y,1E-6) || !NEAR(in.p.z,out.p.z,1E-6)
                 || !NEAR(in.o.r,out.o.r,1E-3) || !NEAR(in.o.p,out.o.p,1E-3) || !NEAR(in.o.y,out.o.y,1E-3) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<orcaice::toString(in)<<endl
                <<"str="<<s<<endl
                <<"out="<<orcaice::toString(out)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toSize2d() and the inverse toString() ... ";
    {
        orca::Size2d in, out;
        orcaice::setSane( in );
        std::string s = orcaice::toString( in );
        int ret = orcaice::toSize2d( s, out );
        if ( ret || !NEAR(in.l,out.l,1E-6) || !NEAR(in.w,out.w,1E-6) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<orcaice::toString(in)<<endl
                <<"str="<<s<<endl
                <<"out="<<orcaice::toString(out)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toSize3d() and the inverse toString() ... ";
    {
        orca::Size3d in, out;
        orcaice::setSane( in );
        std::string s = orcaice::toString( in );
        int ret = orcaice::toSize3d( s, out );
        if ( ret || !NEAR(in.l,out.l,1E-6) || !NEAR(in.w,out.w,1E-6) || !NEAR(in.h,out.h,1E-6) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<orcaice::toString(in)<<endl
                <<"str="<<s<<endl
                <<"out="<<orcaice::toString(out)<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toTimeDuration() and the toStringDuration() ... ";
    {
        orca::Time in, out;
        in.seconds = (73*24*60 + 13*60 + 43)*60 + 13;
        in.useconds = 15;
        std::string s = orcaice::toStringDuration( in );
        int ret = orcaice::toTimeDuration( s, out );
        if ( ret || !NEAR(in.seconds,out.seconds,1E-6) || !NEAR(in.useconds,out.useconds,1E-6) ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<in.seconds<<" "<<in.useconds<<endl
                <<"str="<<s<<endl
                <<"out="<<out.seconds<<" "<<out.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toTimeDuration() with seconds only ... ";
    {
        orca::Time in, out;
        in.seconds = 13;
        in.useconds = 45000;
        std::string s = "13.045";
        int ret = orcaice::toTimeDuration( s, out );
        if ( ret || in != out ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<in.seconds<<" "<<in.useconds<<endl
                <<"str="<<s<<endl
                <<"out="<<out.seconds<<" "<<out.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toTimeDuration() with seconds and minutes ... ";
    {
        orca::Time in, out;
        in.seconds = 73;
        in.useconds = 45000;
        std::string s = "1:13.045";
        int ret = orcaice::toTimeDuration( s, out );
        if ( ret || in != out ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<in.seconds<<" "<<in.useconds<<endl
                <<"str="<<s<<endl
                <<"out="<<out.seconds<<" "<<out.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toTimeDuration() with seconds, minutes, and hours ... ";
    {
        orca::Time in, out;
        in.seconds = 3673;
        in.useconds = 45000;
        std::string s = "1:1:13.045";
        int ret = orcaice::toTimeDuration( s, out );
        if ( ret || in != out ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<in.seconds<<" "<<in.useconds<<endl
                <<"str="<<s<<endl
                <<"out="<<out.seconds<<" "<<out.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toTimeDuration() with negative time ... ";
    {
        orca::Time in, out;
        in.seconds = -3673;
        in.useconds = -45000;
        std::string s = "-1:1:13.045";
        int ret = orcaice::toTimeDuration( s, out );
        if ( ret || in != out ) {
            cout<<"failed ("<<ret<<")"<<endl
                <<"in ="<<in.seconds<<" "<<in.useconds<<endl
                <<"str="<<s<<endl
                <<"out="<<out.seconds<<" "<<out.useconds<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
