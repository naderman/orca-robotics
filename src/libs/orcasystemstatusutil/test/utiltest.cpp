/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcasystemstatusutil/utils.h>
#include <iostream>

using namespace std;

int main(int argc, char * argv[])
{
    cout<<"testing isStatusNovel( subsystem ) ... ";
    {
        orca::SubsystemStatus prevsub;
        prevsub.state = orca::SubsystemWorking;
        prevsub.health = orca::SubsystemOk;
        prevsub.message = "trabajando";
        prevsub.isStalled = false;
        prevsub.sinceHeartbeat = .3222;

        orca::ComponentStatusEpisode prev;
        prev.timeStamp.seconds = 12;
        prev.timeStamp.useconds = 34;
        prev.timeUp = 345;
        prev.state = orca::CompWorking;
        prev.health = orca::CompOk;
        prev.isStalled = 0;
        prev.subsystems["one"] = prevsub;
        prev.repeatCount = 1;

        orca::ComponentStatusEpisode curr = prev;
        curr.timeStamp.seconds = 12;
        curr.timeStamp.useconds = 34;
        curr.timeUp = 543;
        curr.subsystems["one"].sinceHeartbeat = .6677;
        prev.repeatCount = 0;

        if ( orcasystemstatusutil::isStatusNovel(prev,curr) ) {
            cout<<"failed: nothing new in current"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

//     cout<<"testing isStatusNovel() ... ";
//     {
//         orca::ComponentStatusEpisode prev, curr;
//         prevsub.

//         std::string s1 = "platf/comp";
//         orca::FQComponentName fqComp = orcaobj::toComponentName( s1 );
//         std::string s2 = orcaobj::toString( fqComp );
//         if ( s1 != s2 ) {
//             cout<<"failed"<<endl<<"in="<<s1<<"; out="<<s2<<endl;
//             return EXIT_FAILURE;
//         }
//     }
//     cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
