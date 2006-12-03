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

#include <orcaobj/orcaobj.h>

using namespace std;

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    cout<<"testing toComponentName() and toString(orca::FQComponentName) ... ";
    std::string strComp1 = "platf/comp";
    orca::FQComponentName fqComp = orcaice::toComponentName( strComp1 );
    std::string strComp2 = orcaice::toString( fqComp );
    if ( strComp1 != strComp2 ) {
        cout<<"failed"<<endl<<"in="<<strComp1<<"; out="<<strComp2<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing toInterfaceName() and toString() ... ";
    std::string strIface1 = "iface@platf/comp";
    orca::FQInterfaceName fqIface = orcaice::toInterfaceName( strIface1 );
    std::string strIface2 = orcaice::toString( fqIface );
    if ( strIface1 != strIface2 ) {
        cout<<"failed"<<endl<<"in="<<strIface1<<"; out="<<strIface2<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toString() ... ";
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
    
    return EXIT_SUCCESS;
}
