#include "featureextractorbase.h"

#include <orcaiceutil/objutils.h>
#include <iostream>

using namespace orca;
using namespace std;
using namespace orcaiceutil;

FeatureExtractorBase::FeatureExtractorBase()
{
}

FeatureExtractorBase::~FeatureExtractorBase()
{
}

// int 
// LaserDriver::setEnable( bool enableVal )
// {
//     if ( enableVal )
//     {
//         return enable();
//     }
//     else
//     {
//         return disable();
//     }
// }
// 
// bool
// LaserDriver::equal( const orca::LaserConfigPtr &cfg1, const orca::LaserConfigPtr &cfg2 )
// {
//     if ( cfg1->isEnabled == false && cfg2->isEnabled == false ) return true;
// 
//     return ( cfg1->rangeResolution == cfg2->rangeResolution &&
//              cfg1->angleIncrement == cfg2->angleIncrement &&
//              cfg1->isEnabled == cfg2->isEnabled );
// }
// 
// void
// LaserDriver::forceSetConfig( const orca::LaserConfigPtr &desiredConfig )
// {
//     LaserConfigPtr currentConfig = new LaserConfig;
// 
//     while ( true )
//     {
//         cout<<"TRACE(laserdriver.cpp): Configuring laser hardware..." << endl;
//         if ( (setConfig( desiredConfig ) == 0) && (getConfig( currentConfig ) == 0) )
//         {
//             if ( equal( desiredConfig, currentConfig ) )
//             {
//                 cout<<"TRACE(laserdriver.cpp): Success when configuring laser." << endl;
//                 break;
//             }
//         }
//         else
//         {
//             cout<<"TRACE(laserdriver.cpp): Problem configuring laser.  Trying again." << endl;
//             sleep(1);
//         }
//     }
// }
