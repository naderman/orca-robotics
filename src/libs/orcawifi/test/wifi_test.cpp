#include <iostream>
#include <orcawifi/wifiutil.h>

using namespace std;
using namespace wifiutil;

int main()
{
    int numLoops = 0;
            
    while(true)
    {
        try 
        {
            cout << "Testing getting interface names" << endl;
            vector<string> interfaceNames;
            getInterfaceNames( interfaceNames );
            for (uint i=0; i<interfaceNames.size(); i++)
            {
                cout << "Interface " << i << " : " << interfaceNames[i] << endl;
            }
            cout << endl;
            
            cout << "Testing reading data from proc" << endl;
            vector<ProcData> wifiData;
            readFromProc( wifiData );
            printData( wifiData );
            cout << "Reading proc test successful!" << endl << endl;
            
            cout << "Testing reading config" << endl;
            vector<WirelessConfig> configs;
            readConfig( configs );
            printConfig( configs );
            cout << "Reading config test successful" << endl << endl;
            
            cout << "Testing reading data using ioctl" << endl;
            vector<IoctlData> wifiDataIoctl;
            readUsingIoctl( wifiDataIoctl );
            printDataIoctl( wifiDataIoctl );
            cout << "Reading ioctl test successful" << endl;
        }
        catch ( wifiutil::Exception &e )
        {
            if (e.type()==NoWirelessInterface) {
                cout << "We don't have a wireless interface. Test will just pass" << endl;
                return 0;
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


