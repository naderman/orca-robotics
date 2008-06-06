#include "idriver.h"
#include <iostream>
#include <sstream>
#include <orcaobj/orcaobj.h>

using namespace std;

namespace orcalocalnav {

std::string toString( const IDriver::Inputs &i )
{
    stringstream ss;
    ss << endl;
    ss << "  stalled:                 " << i.stalled << endl
       << "  isLocalisationUncertain: " << i.isLocalisationUncertain << endl
       << "  localisePose:            " << i.localisePose << endl
       << "  poseTime:                " << orcaobj::toString(i.poseTime) << endl
       << "  currentVelocity:         " << i.currentVelocity << endl
        // << "  obsRanges:               " << i.obsRanges << endl
       << "  obsTime:                 " << orcaobj::toString(i.obsTime) << endl
       << "  goals: " << endl;
    for ( size_t j=0; j < i.goals.size(); j++ )
    {
        ss << "    " << i.goals[j] << endl;
    }
    return ss.str();
}

}

