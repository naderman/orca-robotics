#include <iostream>
#include <orcawifi/wifiutil.h>

using namespace std;
using namespace wifiutil;

int main()
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
        cout << "Test failed! Reason:" << endl << e.what();
        return -1;
    }

    cout << endl << "Test passed!" << endl;
    return 0;
}