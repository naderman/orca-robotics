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

//#include <orcaiceutil/orcaiceutil.h>
//#include <orca/configutils.h>

using namespace std;
//using namespace orca;

class App : virtual public Ice::Application
{
    public:
        virtual int run(int, char * []);

    private:
        void usage();
};

int App::run( int argc, char* argv[] )
{
    string proxy = "";
    int count = 3;
    int intervalUs = 0;

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
        if ( args[i]=="-h" ) {
            usage();
            return 0;
        } else if ( args[i]=="-r" ) {
            proxy = "IceGrid/Query";
        }
    }

    if ( proxy.empty() ) {
        proxy = args[args.size()-1];
    }

    for( uint i=1; i<args.size()-1; i+=2 )
    {
        if ( args[i]=="-i" ) {
            intervalUs = (int)floor(1.0e6*atof( args[i+1].c_str() ));
        }
        else if ( args[i]=="-c" ) {
            count = atoi( args[i+1].c_str() );
        }
        else if ( args[i]=="-f" ) {
            // already parsed
        }
        else {
            cout<<"warning: unknown command line option "<<args[i]<<endl;
        }
    }

    Ice::ObjectPrx base = communicator()->stringToProxy( proxy );

    Ice::EndpointSeq epoints = base->ice_getEndpoints();
    if ( !epoints.empty() ) {
        cout<<"where : "<<epoints[0]<<endl;
    } else {
        cout<<"no endpoints"<<endl;
    }
    try {
        IceUtil::Time t0 = IceUtil::Time::now();
        IceUtil::Time t1;

        for ( int i=0; i<count; ++i ) {
            base->ice_ping();
            if ( intervalUs ) {
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(intervalUs));
            }
        }
        epoints = base->ice_getEndpoints();
        if ( !epoints.empty() ) {
            cout<<"where : "<<epoints[0]<<endl;
        } else {
            cout<<"no endpoints"<<endl;
        }

        t1 = IceUtil::Time::now();
        IceUtil::Time dt = t1 - t0;

        cout<<endl<<"Ping successful."<<endl;

        cout<<"Proxy\t\t[ "<<base->ice_toString()<<" ]"<<endl;

        cout<<"RTT ("<<count<<") \t[ " << (dt.toMicroSeconds()-count*intervalUs)/count << " us ]" <<endl;

        std::string id = base->ice_id();
        cout<<"Ice ID \t\t[ "<< id << " ]"<< endl;

        Ice::StringSeq ids = base->ice_ids();
        cout<<"All Ice ID's \t[ ";
        for ( uint i=0; i<ids.size(); ++i ) {
            cout << ids[i] << "; ";
        }
        cout<<" ]"<<endl;
    }
    catch(const Ice::Exception& ex)
    {
        cerr << endl << "Ping failed"<< endl;
        cerr << ex << endl;
    }

    cout<<endl;
    return 0;
}

void App::usage()
{
    cout << "USAGE"<<endl;
    cout << "iceping [ -rh ] [ -f file ] [ -c count ] [ -i intervalUs ] proxy"<<endl;
    cout << "  Proxy\tDesitnation specified as a string in the form 'object@adapter'"<<endl;
    cout << "OPTIONS"<<endl;
    cout << "  -c count\n\tPing count times. Default is 3."<<endl;
    cout << "  -f file\n\tUse file as Ice.Config parameter. Default is ~/.orcarc"<<endl;
    cout << "  -h\tPrint this."<<endl;
    cout << "  -i interval\n\tPause for interval seconds after each ping. Default is 0."<<endl;
    cout << "  -r\tPing the IceGridRegistry, specifically its Query interface. This is "<<endl;
    cout << "    \ta way to make sure that the registry is reachable."<<endl;
}


int main(int argc, char * argv[])
{
    bool isModified = false;

    // convert to string sequence for convinience
    Ice::StringSeq args = Ice::argsToStringSeq( argc, argv );

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
