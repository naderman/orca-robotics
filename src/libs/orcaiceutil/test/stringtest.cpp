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
#include <orcaiceutil/exceptions.h>

using namespace std;

int 
main(int argc, char * argv[])
{
    cout<<"testing toString(vector<string>) and toStringSeq() ... ";
    {
        vector<string> in;
//         const char del = ':';
        in.push_back( "a" );
        string s = orcaiceutil::toString( in );
        vector<string> out = orcaiceutil::toStringSeq( s );
        if ( out.size() != in.size() ) {
            cout<<"failed"<<endl
                <<"wrong output size: expect="<<in.size()<<", got="<<out.size()<<endl
                <<"string="<<s<<endl;
            return EXIT_FAILURE;
        }

        // with two elemens
        in.push_back( "bbb" );
        s = orcaiceutil::toString( in );
        out = orcaiceutil::toStringSeq( s );
        if ( out.size() != in.size() ) {
            cout<<"failed"<<endl
                <<"wrong output size: expect="<<in.size()<<", got="<<out.size()<<endl
                <<"string="<<s<<endl;
            return EXIT_FAILURE;
        }
        if ( out[0]!=in[0] || out[1]!=in[1] ) {
            cout<<"failed, wrong output: string="<<s<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toStringSeq() and toString(vector<string>) ... ";
    {
        string s1 = "alpha:bravo:charlie:delta";
        vector<string> seq = orcaiceutil::toStringSeq( s1 );
        string s2 = orcaiceutil::toString( seq );
        if ( s1 != s2 ) {
            cout<<"failed"<<endl<<"in="<<s1<<"; out="<<s2<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toLowerCase() ... ";
    {
        string in = "qwertyQWERTY123,./";
        string out = orcaiceutil::toLowerCase( in );
        string expect = "qwertyqwerty123,./";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toLowerCase() with empty string ... ";
    {
        string in = "";
        string out = orcaiceutil::toLowerCase( in );
        string expect = "";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toUpperCase() ... ";
    {
        string in = "qwertyQWERTY123,./";
        string out = orcaiceutil::toUpperCase( in );
        string expect = "QWERTYQWERTY123,./";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toUpperCase() with empty string ... ";
    {
        string in = "";
        string out = orcaiceutil::toUpperCase( in );
        string expect = "";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with empty string ... ";
    {
        string s = "";
        vector<string> parameters;
        map<string,string> values, defaults;
        orcaiceutil::substitute( s, parameters, values, defaults );
        string expect = "";
        if ( s != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<s<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with non-existent parameter ... ";
    {
        string s = "jfkdj ${var}fd fjkdjf";
        vector<string> parameters;
        map<string,string> values, defaults;
        try {
            orcaiceutil::substitute( s, parameters, values, defaults );
            cout<<"failed, expect to catch exception"<<endl;
            return EXIT_FAILURE;
        }
        catch ( const orcaiceutil::Exception& e ) {}
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with no matching values ... ";
    {
        string s = "jfkdj ${var}fd fjkdjf";
        vector<string> parameters;
        parameters.push_back( "var" );
        map<string,string> values, defaults;
        values["crap"] = "fdkj";
        defaults["crud"] = "jkljk";
        try {
            orcaiceutil::substitute( s, parameters, values, defaults );
            cout<<"failed, expect to catch exception"<<endl;
            return EXIT_FAILURE;
        }
        catch ( const orcaiceutil::Exception& e ) {}
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with provided value ... ";
    {
        string s = "jfkdj ${var}fd fjkdjf";
        vector<string> parameters;
        parameters.push_back( "var" );
        map<string,string> values, defaults;
        values["var"] = "Orca";
        defaults["crud"] = "jkljk";
        orcaiceutil::substitute( s, parameters, values, defaults );
        string expect = "jfkdj Orcafd fjkdjf";
        if ( s != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<s<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with default value ... ";
    {
        string s="The best framework is ${framework}";
        vector<string> parameters;
        parameters.push_back( "framework" );
        map<string,string> values, defaults;
        defaults["framework"] = "Orca";
        orcaiceutil::substitute( s, parameters, values, defaults );
        string expect = "The best framework is Orca";
        if ( s != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<s<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with value and default value ... ";
    {
        string s="The best framework is ${framework}";
        vector<string> parameters;
        parameters.push_back( "framework" );
        map<string,string> values, defaults;
        values["framework"] = "Orca-3";
        defaults["framework"] = "Orca";
        orcaiceutil::substitute( s, parameters, values, defaults );
        string expect = "The best framework is Orca-3";
        if ( s != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<s<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing substitute() with 2 parameters ... ";
    {
        string s="The best ${framework} is ${project}";
        vector<string> parameters;
        parameters.push_back( "project" );
        parameters.push_back( "framework" );
        map<string,string> values, defaults;
        values["project"] = "orca-robotics";
        defaults["framework"] = "Orca";
        orcaiceutil::substitute( s, parameters, values, defaults );
        string expect = "The best Orca is orca-robotics";
        if ( s != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<s<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with padding ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, 7 );
        string expect = "   abcd";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with non-standard padding ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, 7, 'x' );
        string expect = "xxxabcd";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with left adjustment ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, 7, 'x', true );
        string expect = "abcdxxx";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with trunkating ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, 3 );
        string expect = "abc";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with trunkating to nothing ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, 0 );
        string expect = "";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with no change ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, 4 );
        string expect = in;
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with no empty string ... ";
    {
        string in = "";
        string out = orcaiceutil::toFixedWidth( in, 4 );
        string expect = "    ";
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing toFixedWidth() with negative width ... ";
    {
        string in = "abcd";
        string out = orcaiceutil::toFixedWidth( in, -4 );
        string expect = in;
        if ( out != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<out<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    string dir = orcaiceutil::pathDelimeter()+"path"+orcaiceutil::pathDelimeter();

    cout<<"testing basename() typical ... ";
    {
        string path = dir+"file.ext";
        string filename = orcaiceutil::basename( path );
        string expect = "file.ext";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() no filename ... ";
    {
        string path = dir;
        string filename = orcaiceutil::basename( path );
        string expect = "";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention ... ";
    {
        string path = dir+"file.ext";
        string filename = orcaiceutil::basename( path, true );
        string expect = "file";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention, no extention ... ";
    {
        string path = dir+"file";
        string filename = orcaiceutil::basename( path, true );
        string expect = "file";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing basename() remove extention, 2 extentions ... ";
    {
        string path = dir+"file.crap.ext";
        string filename = orcaiceutil::basename( path, true );
        string expect = "file.crap";
        if ( filename != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<filename<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing dirname() typical ... ";
    {
        string dir = orcaiceutil::pathDelimeter()+"path";
        string path = dir+orcaiceutil::pathDelimeter()+"file.ext";
        string dname = orcaiceutil::dirname( path );
        string expect = dir;
        if ( dname != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<dname<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    cout<<"testing dirname() no dir ... ";
    {
        string path = "file.ext";
        string dname = orcaiceutil::dirname( path );
        string expect = ".";
        if ( dname != expect ) {
            cout<<"failed"<<endl<<"expect '"<<expect<<"' got='"<<dname<<"'"<<endl;
            return EXIT_FAILURE;
        }
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
