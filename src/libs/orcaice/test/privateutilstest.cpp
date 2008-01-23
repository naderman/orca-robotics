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
#include <orcaice/detail/privateutils.h>
#include <hydroutil/mathdefs.h>
// #include <orcaobj/stringutils.h>

using namespace std;

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty( "Common", "old" );

    Ice::PropertiesPtr extras = Ice::createProperties();
    extras->setProperty( "Common", "new" );
    extras->setProperty( "Uncommon", "new" );
    extras->setProperty( "Uncommon2", "new" );

    int ret;
    std::string retString;

    cout<<"testing transferProperty() with existing key and NO force ... ";
    orcaice::detail::transferProperty( extras, props, "Common", "Common", false );
    ret = orcaice::getProperty( props, "Common", retString );
    if ( ret!=0 || retString!="old" ) {
        cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<retString<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing transferProperty() with existing key and force ... ";
    orcaice::detail::transferProperty( extras, props, "Common", "Common", true );
    ret = orcaice::getProperty( props, "Common", retString );
    if ( ret!=0 || retString!="new" ) {
        cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<retString<<endl;
        return EXIT_FAILURE;
    }

    cout<<"ok"<<endl;
    cout<<"testing transferProperty() with NON-existing key and NO force ... ";
    orcaice::detail::transferProperty( extras, props, "Uncommon", "Uncommon", false );
    ret = orcaice::getProperty( props, "Uncommon", retString );
    if ( ret!=0 || retString!="new" ) {
        cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<retString<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing transferProperty() with NON-existing key and force ... ";
    orcaice::detail::transferProperty( extras, props, "Uncommon2", "Uncommon2", true );
    ret = orcaice::getProperty( props, "Uncommon2", retString );
    if ( ret!=0 || retString!="new" ) {
        cout<<"failed"<<endl<<"\texisting key with string: ret="<<ret<<"; value="<<retString<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
