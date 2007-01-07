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

#include <orcaice/stringutils.h>
#include <orcaice/sysutils.h>

using namespace std;

int 
main(int argc, char * argv[])
{
    cout<<"testing toLowerCase() ... ";
    std::string in = "qwertyQWERTY123,./";
    std::string out = orcaice::toLowerCase( in );
    std::string expect = "qwertyqwerty123,./";
    if ( out != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toLowerCase() with empty string ... ";
    in = "";
    out = orcaice::toLowerCase( in );
    expect = "";
    if ( out != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toUpperCase() ... ";
    in = "qwertyQWERTY123,./";
    out = orcaice::toUpperCase( in );
    expect = "QWERTYQWERTY123,./";
    if ( out != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing toUpperCase() with empty string ... ";
    in = "";
    out = orcaice::toUpperCase( in );
    expect = "";
    if ( out != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    std::string dir = orcaice::pathDelimeter()+"path"+orcaice::pathDelimeter();

    cout<<"testing basename() typical ... ";
    std::string path = dir+"file.ext";
    std::string filename = orcaice::basename( path );
    expect = "file.ext";
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() no filename ... ";
    path = dir;
    filename = orcaice::basename( path );
    expect = "";
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention ... ";
    path = dir+"file.ext";
    filename = orcaice::basename( path, true );
    expect = "file";
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention, no extention ... ";
    path = dir+"file";
    filename = orcaice::basename( path, true );
    expect = "file";
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention, 2 extentions ... ";
    path = dir+"file.crap.ext";
    filename = orcaice::basename( path, true );
    expect = "file.crap";
    if ( filename != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing dirname() typical ... ";
    dir = orcaice::pathDelimeter()+"path";
    path = dir+orcaice::pathDelimeter()+"file.ext";
    std::string dname = orcaice::dirname( path );
    expect = dir;
    if ( dname != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<dname<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing dirname() no dir ... ";
    path = "file.ext";
    dname = orcaice::dirname( path );
    expect = ".";
    if ( dname != expect ) {
        cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<dname<<"'"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
