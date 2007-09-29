/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaiceutil/stringutils.h>
#include <orcaiceutil/sysutils.h>

using namespace std;

int 
main(int argc, char * argv[])
{
    cout<<"testing toLowerCase() ... ";
    {
        std::string in = "qwertyQWERTY123,./";
        std::string out = orcaiceutil::toLowerCase( in );
        std::string expect = "qwertyqwerty123,./";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toLowerCase() with empty string ... ";
    {
        std::string in = "";
        std::string out = orcaiceutil::toLowerCase( in );
        std::string expect = "";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toUpperCase() ... ";
    {
        std::string in = "qwertyQWERTY123,./";
        std::string out = orcaiceutil::toUpperCase( in );
        std::string expect = "QWERTYQWERTY123,./";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toUpperCase() with empty string ... ";
    {
        std::string in = "";
        std::string out = orcaiceutil::toUpperCase( in );
        std::string expect = "";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with padding ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, 7 );
        std::string expect = "   abcd";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with non-standard padding ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, 7, 'x' );
        std::string expect = "xxxabcd";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with left adjustment ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, 7, 'x', true );
        std::string expect = "abcdxxx";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with trunkating ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, 3 );
        std::string expect = "abc";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with trunkating to nothing ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, 0 );
        std::string expect = "";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with no change ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, 4 );
        std::string expect = in;
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with no empty string ... ";
    {
        std::string in = "";
        std::string out = orcaiceutil::toFixedWidth( in, 4 );
        std::string expect = "    ";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with negative width ... ";
    {
        std::string in = "abcd";
        std::string out = orcaiceutil::toFixedWidth( in, -4 );
        std::string expect = in;
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    std::string dir = orcaiceutil::pathDelimeter()+"path"+orcaiceutil::pathDelimeter();

    cout<<"testing basename() typical ... ";
    {
        std::string path = dir+"file.ext";
        std::string filename = orcaiceutil::basename( path );
        std::string expect = "file.ext";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() no filename ... ";
    {
        std::string path = dir;
        std::string filename = orcaiceutil::basename( path );
        std::string expect = "";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention ... ";
    {
        std::string path = dir+"file.ext";
        std::string filename = orcaiceutil::basename( path, true );
        std::string expect = "file";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention, no extention ... ";
    {
        std::string path = dir+"file";
        std::string filename = orcaiceutil::basename( path, true );
        std::string expect = "file";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention, 2 extentions ... ";
    {
        std::string path = dir+"file.crap.ext";
        std::string filename = orcaiceutil::basename( path, true );
        std::string expect = "file.crap";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing dirname() typical ... ";
    {
        std::string dir = orcaiceutil::pathDelimeter()+"path";
        std::string path = dir+orcaiceutil::pathDelimeter()+"file.ext";
        std::string dname = orcaiceutil::dirname( path );
        std::string expect = dir;
        if ( dname != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<dname<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing dirname() no dir ... ";
    {
        std::string path = "file.ext";
        std::string dname = orcaiceutil::dirname( path );
        std::string expect = ".";
        if ( dname != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<dname<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
