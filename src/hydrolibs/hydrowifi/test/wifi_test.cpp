#include <iostream>
#include <hydrowifi/wifiutil.h>

using namespace std;
using namespace wifiutil;

int main()
{
    int numLoops = 0;
    bool haveIoctl = true;
            
    while(true)
    {
        try 
        {
            cout << "Testing getting interface names" << endl;
            vector<string> interfaceNames;
            getInterfaceNames( interfaceNames );
            for (unsigned int i=0; i<interfaceNames.size(); i++)
            {
                cout << "Interface " << i << " : " << interfaceNames[i] << endl;
            }
            cout << endl;
            
            cout << "Testing reading data from proc" << endl;
            vector<ProcData> wifiData;
            readFromProc( wifiData );
            cout << "Result: " << endl << toString( wifiData );
            cout << "Reading proc test successful!" << endl << endl;
            
            if (haveIoctl)
            {
                cout << "Testing reading config" << endl;
                vector<WirelessConfig> configs;
                readConfig( configs );
                cout << "Result: " << endl << toString( configs );
                cout << "Reading config test successful" << endl << endl;
                
                cout << "Testing reading data using ioctl" << endl;
                vector<IoctlData> wifiDataIoctl;
                readUsingIoctl( wifiDataIoctl );
                cout << "Result: " << endl << toString( wifiDataIoctl );
                cout << "Reading ioctl test successful" << endl;
            }
        }
        catch ( wifiutil::Exception &e )
        {
            if (e.type()==NoWirelessInterface) {
                cout << "We don't have a wireless interface. Test will just pass" << endl;
                return 0;
            } else if (e.type()==IoctlProblem) {
                cout << "Hardware does not seem to support ioctl access. Will only test proc." << endl;
                haveIoctl = false;
            } else {
                cout << "Test failed! We've done " << numLoops << " loops. More info: " << e.what();
                return -1;
            }
        }
        
        numLoops++ ;
        if (numLoops==2000) break;
    }

    cout << endl << "Test passed!" << endl;
    return 0;
}


