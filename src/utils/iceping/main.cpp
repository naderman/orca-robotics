/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <math.h>

#include <Ice/Ice.h>
#include <IceUtil/Time.h>

using namespace std;

void usage()
{
    cout << "USAGE"<<endl;
    cout << "iceping [ -orhvV ] [ -f file ] [ -c count ] [ -i intervalUs ] proxy"<<endl;
    cout << "  Proxy\t interface identity in the form 'interface@platform/component'"<<endl;
    cout << "    \tIn Ice terms this is a string in the form 'object@adapter'"<<endl;
    cout << "OPTIONS"<<endl;
    cout << "  -o component\n\tPings the Home interface of component, i.e. 'home@platform/component'"<<endl;
    cout << "    \tRelies on the fact that all Orca components provide Home interface"<<endl;
    cout << "  -r registry\n\tPings the IceGrid Registry as described in configuration files."<<endl;
    cout << "    \tSpecifically, pings the Query interface."<<endl;
    cout << "  -c count\n\tPing count times. Default is 3."<<endl;
    cout << "  -f file\n\tUse file as Ice.Config parameter. Default is ~/.orcarc"<<endl;
    cout << "  -h help\n\tPrints this."<<endl;
    cout << "  -v version\n\tPrints Ice version."<<endl;
    cout << "  -V verbose\n\tPrints extra debugging information."<<endl;
    cout << "  -i interval\n\tPause for interval seconds after each ping. Default is 0."<<endl;
    cout << "EXAMPLE"<<endl;
    cout << "  iceping -r -V -c 5 -i 1    : pings the registry 5 times with 1 sec interval in verbose mode."<<endl;
    cout << "  iceping home@platf/comp    : pings the Home interface of component 'comp' on platform 'platf'"<<endl;
    cout << "  iceping -o platf/comp      : same as above"<<endl;
}

class App : virtual public Ice::Application
{
    public:
        virtual int run(int, char * []);
};

int App::run( int argc, char* argv[] )
{
    string proxy = "";
    int count = 3;
    int intervalUs = 0;
    bool verbose = false;

    // Convert argc/argv to a string sequence.
    Ice::StringSeq args = Ice::argsToStringSeq(argc, argv);
    //cout<<args.size()<<" parameters"<<endl;

    // need at least one parameter: the proxy name
    if ( args.size()<2 ) {
        cout<<"Must enter at least one argument: the proxy name"<<endl;
        usage();
        return 0;
    }

    // go through value-less keys
    for( uint i=1; i<args.size(); ++i )
    {
        if ( args[i]=="-r" ) {
            proxy = "IceGrid/Query";            
        }
        else if ( args[i]=="-V" ) {
            verbose = true;
        }
    }

    // proxy must've been supplied as the last command line parameter
    if ( proxy.empty() ) {
        proxy = args[args.size()-1];
    }

    // this option prepends something to proxy
    for( uint i=1; i<args.size(); ++i )
    {
        if ( args[i]=="-o" ) {
            proxy = "home@"+proxy;
        }
    }

    for( uint i=1; i<args.size()-1; ++i )
    {
        if ( args[i]=="-i" ) {
            intervalUs = (int)floor(1.0e6*atof( args[i+1].c_str() ));
            ++i;
        }
        else if ( args[i]=="-c" ) {
            count = atoi( args[i+1].c_str() );
            ++i;
        }
        else if ( args[i]=="-f" ) {
            // already parsed
        }
        else if ( args[i]=="-o" ) {
            // already parsed
        }
        else if ( args[i]=="-r" ) {
            // already parsed
        }
        else if ( args[i]=="-V" ) {
            // already parsed
        }
        else {
            cout<<"warning: unknown command line option "<<args[i]<<endl;
        }
    }

    // got all parameters
    if ( verbose ) {
        cout<<"will ping "<<proxy<<" "<<count<<" times with "<<intervalUs<<"us interval"<<endl;
        
        std::string locatorStr = communicator()->getProperties()->getProperty( "Ice.Default.Locator" );
        cout<<"will use locator:  "<<locatorStr<<endl;

        // this is copy of orcaice::printConmponentProperties() from orcaice/privateutils.cpp
        Ice::StringSeq propSeq = communicator()->getProperties()->getCommandLineOptions();
        // strip off all orca-specific properties, we don't use them anyway
        propSeq = communicator()->getProperties()->parseCommandLineOptions( "Orca", propSeq );

        std::ostringstream os;
        os << propSeq.size();
    
        cout<<"iceping properties ("<<os.str()<<")"<<endl;
        cout<<"========================"<<endl;
    
        for ( uint i=0; i<propSeq.size(); ++i ) {
            cout<<propSeq[i]<<endl;
        }
        cout<<"========================"<<endl;
    }
    
    Ice::ObjectPrx base = communicator()->stringToProxy( proxy );

    try {
        IceUtil::Time t0 = IceUtil::Time::now();
        IceUtil::Time t1;

        for ( int i=0; i<count; ++i ) {
            base->ice_ping();
            if ( intervalUs ) {
                IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(intervalUs));
            }
        }
        // stop the clock
        t1 = IceUtil::Time::now();
        IceUtil::Time dt = t1 - t0;

        cout<<endl<<"Ping successful."<<endl;

        // we must've connected, otherwise we'd have an exception
        Ice::ConnectionPtr conn = base->ice_connection();
        cout<<"Proxy\t\t[ "<<base->ice_toString()<<" ]"<<endl;
        
        cout<<"Connection\t[ "<<conn->toString()<<" ]"<<endl;

        cout<<"RTT ("<<count<<") \t[ " << (dt.toMicroSeconds()-count*intervalUs)/count << " us ]" <<endl;

        std::string id = base->ice_id();
        cout<<"Ice ID \t\t[ "<< id << " ]"<< endl;

        if ( verbose ) {
            Ice::StringSeq ids = base->ice_ids();
            cout<<"All Ice ID's \t[ ";
            for ( uint i=0; i<ids.size(); ++i ) {
                cout << ids[i] << "; ";
            }
            cout<<" ]"<<endl;
        }
    }
    catch(const Ice::Exception& ex)
    {
        cerr << endl << "Ping failed."<<endl;
        cerr << ex << endl;
    }

    cout<<endl;
    return 0;
}

int main(int argc, char * argv[])
{
    bool isModified = false;

    // basic info before we do anything
    cout<< "Ice version: "<<ICE_STRING_VERSION<<endl;

    // convert to string sequence for convinience
    Ice::StringSeq args = Ice::argsToStringSeq( argc, argv );

    // get the quick and easy ones out of the way
    for( uint i=1; i<args.size(); ++i )
    {
        if ( args[i]=="-h" ) {
            usage();
            return 0;
        }
        else if ( args[i]=="-v" ) {
            // alrady printed out the version
            return 0;
        }
    }

    // find the property file
    string propertiesFile;
    if ( args.size() > 2 ) {
        for( uint i=1; i<args.size(); ++i )
        {
            if ( args[i]=="-f" ) {
                propertiesFile = args[i+1];
            }
        }
    }
    // Find the global property file
    if ( propertiesFile.empty() ) {
        //! @todo Linux only!
        char *home = getenv("HOME");
        if ( home == NULL ) {
            cout<<"warning: environment variable 'HOME' is not set, while trying to load .orcarc"<<endl;
        }
        else {
            // Load its properties
            propertiesFile = home;
            propertiesFile += "/.orcarc";
        }
    }

    if ( !propertiesFile.empty() ) {
        args.push_back( "--Ice.Config=" + propertiesFile );
        cout<<"assumed config file: --Ice.Config="<<propertiesFile<<endl;
        isModified = true;
    }

    // if nothing has changed, don't bother converting
    if ( !isModified ) {
        App app;
        return app.main(argc, argv);
    }

    // permanent storage of modified command line arguments
    int argc2;
    char** argv2;
    
    // convert new command line options to argc/argv format
    argc2 = args.size();
    
    // argv must have one more element than needed, the last one set to null
    // see ice manual sec.27.6
    argv2 = new char*[argc2+1];
    argv2[argc2] = 0;
    
    for ( int i=0; i<argc2; ++i ) {
        argv2[i] = new char[args[i].size()+1];
        strcpy( argv2[i], args[i].data() );
    }

    App app;
    return app.main(argc2, argv2);
}
