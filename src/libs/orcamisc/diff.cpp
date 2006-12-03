/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "diff.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace orcamisc {

    bool outputMatchesReferenceFile( const std::string &outputString,
                                     const std::string &referenceFileName )
    {
        const std::string testFileName = "test_output.txt";

        // Write it to a file
        std::ofstream testFile( testFileName.c_str() );
        if ( !testFile )
        {
            cout << "ERROR(diff.cpp): Couldn't open test file '" << testFileName << "'" << endl;
            return false;
        }
        testFile << outputString;
        testFile.close();

        // And compare it against the reference output
        if ( !filesAreSame( testFileName, referenceFileName ) )
        {
            cout << "ERROR(diff.cpp): Output was different from reference!" << endl;
            std::ifstream referenceFile( referenceFileName.c_str() );
            if ( !referenceFile )
            {
                cout << "ERROR(diff.cpp): Couldn't open reference file '" << referenceFileName << "'" << endl;
                return false;
            }
            cout << "ERROR(diff.cpp): reference file was: " << endl;
            while ( !referenceFile.eof() )
            {
                std::string line;
                referenceFile >> line;
                cout << line;
            }
            referenceFile.close();
            return false;
        }
        else
        {
            return true;
        }
    }

    bool filesAreSame( const std::string &filename1,
                       const std::string &filename2 )
    {
        FILE *f1 = fopen( filename1.c_str(), "r" );
        FILE *f2 = fopen( filename2.c_str(), "r" );

        if ( f1 == NULL )
        {
            cout << "ERROR(diff.cpp): Couldn't open '" << filename1 << "'" << endl;
            return false;
        }
        if ( f2 == NULL )
        {
            cout << "ERROR(diff.cpp): Couldn't open '" << filename2 << "'" << endl;
            fclose(f1);
            return false;
        }

        bool problem = false;
        char byte1, byte2;
        while ( true )
        {
            int ret1 = fread( &byte1, sizeof(char), 1, f1 );
            int ret2 = fread( &byte2, sizeof(char), 1, f2 );
            if ( ret1 != ret2 )
            {
                problem = true;
                break;
            }
            if ( byte1 != byte2 )
            {
                problem = true;
                break;
            }
            if ( ret1 == 0 )
            {
                // Finished file1.
                break;
            }
            if ( ret1 != 1 && ret1 != 0 )
            {
                cout << "ERROR(diff.cpp): Got unexpected return value: " << ret1 << endl;
                exit(1);
            }
        }
        
        fclose(f1);
        fclose(f2);
        if ( problem )
            return false;
        else
            return true;
    }
}
