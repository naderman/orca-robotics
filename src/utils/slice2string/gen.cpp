/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "gen.h"
#include <Slice/CPlusPlusUtil.h>
// #include <IceUtil/Functional.h>
#include <IceUtil/Iterator.h>
#include <Slice/Checksum.h>

// #include <limits>
// #include <sys/stat.h>

using namespace std;
using namespace Slice;
using namespace IceUtil;

namespace detail {

void printHeader( ::IceUtil::Output& out )
{
    out << "/*";
    out << "\n * Auto-generated by slice2string.";
    out << "\n * Do not modify or commit to source repository.";
    out << "\n */";
    out << "\n";
}

}

Slice::Gen::Gen(const string& name, const string& base, const string& headerExtension,
                const string& sourceExtension, const vector<string>& extraHeaders, const string& include,
                const vector<string>& includePaths, const string& dllExport, const string& dir,
                bool imp, bool checksum, bool stream, bool ice,
        // custom option
        const std::string& module) :
    _base(base),
    _headerExtension(headerExtension),
    _sourceExtension(sourceExtension),
    _extraHeaders(extraHeaders),
    _include(include),
    _includePaths(includePaths),
    _dllExport(dllExport),
    _impl(imp),
    _checksum(checksum),
    _stream(stream),
    _ice(ice),
    _module(module)
{
    for(vector<string>::iterator p = _includePaths.begin(); p != _includePaths.end(); ++p)
    {
        if(p->length() && (*p)[p->length() - 1] != '/')
        {
            *p += '/';
        }
    }

    string::size_type pos = _base.find_last_of("/\\");
    if(pos != string::npos)
    {
        _base.erase(0, pos + 1);
    }

    string fileH = _base + "." + _headerExtension;
    string fileC = _base + "." + _sourceExtension;
    if(!dir.empty())
    {
        fileH = dir + '/' + fileH;
        fileC = dir + '/' + fileC;
    }

    H.open(fileH.c_str());
    if(!H)
    {
        cerr << name << ": can't open `" << fileH << "' for writing" << endl;
        return;
    }
    
    C.open(fileC.c_str());
    if(!C)
    {
        cerr << name << ": can't open `" << fileC << "' for writing" << endl;
        return;
    }

    detail::printHeader(H);
    detail::printHeader(C);
    H << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
    C << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";

    string s = fileH;
    if(_include.size())
    {
        s = _include + '/' + s;
    }
    transform(s.begin(), s.end(), s.begin(), ToIfdef());
    H << "\n#ifndef __" << s << "__";
    H << "\n#define __" << s << "__";
    H << '\n';
}

Slice::Gen::~Gen()
{
    H << nl << eb;
    H << "\n\n#endif\n";

    C << '\n';
    C << "\n}";
    C << '\n';
}

bool
Slice::Gen::operator!() const
{
    if(!H || !C)
    {
        return true;
    }
    return false;
}

void
Slice::Gen::generate(const UnitPtr& p)
{    
    writeExtraHeaders(C);

//     string lib_namespace = _module + "ifacestring";
    string lib_namespace = "ifacestring";
    string lib_dir = _module + "ifacestring";

    H << "\n#include <string>";
    H << "\n#include <" << _module << "/" << _base << ".h>";    
    // includeFiles contains full paths. we want just the base.
    StringList includes = p->includeFiles();
    for(StringList::const_iterator q = includes.begin(); q != includes.end(); ++q)
    { 
        // remove everything except the base
        string include_base = *q;
        string::size_type pos = include_base.find_last_of("/\\");
        if(pos != string::npos)
        {
            include_base.erase(0, pos + 1);
        }    
        if((pos = include_base.rfind('.')) != string::npos)
        {
            include_base.erase(pos);
        }
        // remove everything except the module
        string include_module = *q;
cout<<include_module<<endl;
        string::size_type pos_mod = include_module.find_last_of("/\\");
        if(pos_mod != string::npos)
        {
            // remove base
            include_module.erase(pos_mod, string::npos);
cout<<include_module<<endl;
            string::size_type pos_dir = include_module.find_last_of("/\\");
            if(pos_dir != string::npos) {
                // remove dir
                include_module.erase(0, pos_dir+1);
            }
        }    
cout<<include_module<<endl;
        string include_namespace = include_module + "ifacestring";

        H << "\n#include <" << include_namespace << "/" << include_base << "." << _headerExtension << ">";
    }
    H << "\n";
    H << "\nnamespace " << lib_namespace;
    H << "\n{";
    H << "\n";

    C << "\n#include <" << lib_dir << "/" << _base << ".h>";
    C << "\n#include <gbxsickacfr/gbxutilacfr/exceptions.h>";
    C << "\n#include <sstream>";
    C << "\n#include \"util.h\"";
    C << "\n";
    C << "\nusing namespace std;";
    C << "\n";
    C << "\nnamespace " << lib_namespace;
    C << "\n{";

    TypesVisitor typesVisitor(H, C, _dllExport, _stream);
    p->visit(&typesVisitor, false);
}

void
Slice::Gen::writeExtraHeaders(IceUtil::Output& out)
{
    for(vector<string>::const_iterator i = _extraHeaders.begin(); i != _extraHeaders.end(); ++i)
    {
        string hdr = *i;
        string guard;
        string::size_type pos = hdr.rfind(',');
        if(pos != string::npos)
        {
            hdr = i->substr(0, pos);
            guard = i->substr(pos + 1);
        }
        if(!guard.empty())
        {
            out << "\n#ifndef " << guard;
            out << "\n#define " << guard;
        }
        out << "\n#include <";
        if(!_include.empty())
        {
            out << _include << '/';
        }
        out << hdr << '>';
        if(!guard.empty())
        {
            out << "\n#endif";
        }
    }
}

Slice::Gen::TypesVisitor::TypesVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream), _doneStaticSymbol(false), _useWstring(false)
{
}

bool
Slice::Gen::TypesVisitor::visitModuleStart(const ModulePtr& p)
{
//     _useWstring = setUseWstring(p, _useWstringHist, _useWstring);
//     H << sp << nl << "namespace " << name << nl << '{';
    return true;
}

void
Slice::Gen::TypesVisitor::visitModuleEnd(const ModulePtr& p)
{
//     H << sp << nl << '}';
//     _useWstring = resetUseWstring(_useWstringHist);
}

bool
Slice::Gen::TypesVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if ( p->declaration()->isInterface() )
        return false;

    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    
    H << "\nstd::string toString( const " << scope.substr(2) << name << "Ptr& obj, bool expand=true, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2) << name << "Ptr& objPtr, bool expand, int indent )";
    C << sb;
    C << nl << scope.substr(2) << name << "& obj = *objPtr;";
    C << nl << "string ind;";
    C << nl << "for ( int i=0; i<indent; ++i ) ind += ' ';";
    C << nl << "ostringstream ss;";

    return true;
}

void
Slice::Gen::TypesVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    C << nl << "return ss.str();";
    C << eb;
}

bool
Slice::Gen::TypesVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    return false;
}

void
Slice::Gen::TypesVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
}

void
Slice::Gen::TypesVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    EnumeratorList enumerators = p->getEnumerators();

    H << nl << "std::string toString( const " << scope.substr(2) << name << ", bool expand=true, int indent=0 );";

    C << nl;
    C << nl << "string";
    C << nl << "toString( const " << scope.substr(2) << name << " obj, bool expand, int indent )";
    C << sb;
    C << nl << "switch ( obj )";
    C << sb;

    EnumeratorList::const_iterator en = enumerators.begin();
    while(en != enumerators.end())
    {
        C << nl << "case " << scope.substr(2) << fixKwd((*en)->name()) << " :";
        C << nl << "\treturn \"" << fixKwd((*en)->name()) << "\";";
        ++en;
    }

    C << eb;
    C << nl << "ostringstream ss;";
    C << nl << "ss << \"Unknown case in enumerator " << scope.substr(2) << name << ": \"<< ((int)obj);";
    C << nl << "throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );";

    C << eb;
}

bool
Slice::Gen::TypesVisitor::visitStructStart(const StructPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    
    H << "\nstd::string toString( const " << scope.substr(2) << name << "& obj, bool expand=true, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2) << name << "& obj, bool expand, int indent )";
    C << sb;
    C << nl << "string ind;";
    C << nl << "for ( int i=0; i<indent; ++i ) ind += ' ';";
    C << nl << "ostringstream ss;";
    return true;
}

void
Slice::Gen::TypesVisitor::visitStructEnd(const StructPtr& p)
{       
    C << nl << "return ss.str();";
    C << eb;
}

void
Slice::Gen::TypesVisitor::visitDataMember(const DataMemberPtr& p)
{
    string name = fixKwd(p->name());

//     if ( p->containedType() == Contained::ContainedTypeSequence ) {
// cout<<"*** This is a sequence : "<<name<<endl;
//         string s = typeToString(p->type(), _useWstring, p->getMetaData());
//         C << nl << "ss << endl << ind << \"" << name << " = \" << seqToString<" << s << ">( obj." << name << ", expand, indent+2 );";
//     }
//     else {
// cout<<"*** This is NOT a sequence : "<<name<<" type="<<p->containedType()<<" kind="<<p->kindOf()<<endl;
        C << nl << "ss << endl << ind << \"" << name << " = \" << toString( obj." << name << ", expand, indent+2 );";
//     }
}

void
Slice::Gen::TypesVisitor::visitSequence(const SequencePtr& p)
{    
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

    H << nl << "std::string toString( const " << scope.substr(2) << name << "& obj, bool expand=true, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2) << name << "& obj, bool expand, int indent )";
    C << sb;

//     string s = typeToString(p->type(), _useWstring, p->getMetaData());
    C << nl << "return seqToString< " << scope.substr(2) << name << " >( obj, expand, indent+2 );";

//     C << nl << "ostringstream ss;";
//     C << nl << "ss << \"sequence (\" << obj.size() << \")\";";
// 
//     C << nl << "if ( expand )";
//     C << sb;
//     C << nl << "string ind;";
//     C << nl << "for ( int i=0; i<indent; ++i )";
//     C << nl << "    ind += ' ';";
//     C << nl << "for ( size_t i=0; i<obj.size(); ++i )";
//     C << nl << "    ss << endl << ind << i << \"\\t\" << toString( obj[i], expand, indent+2 );";
//     C << eb;
// 
//     C << nl << "return ss.str();";
    C << eb;
}

void
Slice::Gen::TypesVisitor::visitDictionary(const DictionaryPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

    H << nl << "std::string toString( const " << scope.substr(2) << name << "& obj, bool expand=true, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2) << name << "& obj, bool expand, int indent )";
    C << sb;
    C << nl << "string ind;";
//     C << nl << "for ( int i=0; i<indent; ++i ) ind += ' ';";
    C << nl << "ostringstream ss;";
    C << nl << "ss << \"dict (\" << obj.size() << \")\";";
    C << nl << "return ss.str();";
    C << eb;
}

void
Slice::Gen::TypesVisitor::visitConst(const ConstPtr& p)
{
}

void
Slice::Gen::TypesVisitor::emitUpcall(const ExceptionPtr& base, const string& call, bool isLocal)
{
    C.zeroIndent();
    C << nl << "#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug"; // COMPILERBUG
    C.restoreIndent();
    C << nl << (base ? fixKwd(base->name()) : string(isLocal ? "LocalException" : "UserException")) << call;
    C.zeroIndent();
    C << nl << "#else";
    C.restoreIndent();
    C << nl << (base ? fixKwd(base->scoped()) : string(isLocal ? "::Ice::LocalException" : "::Ice::UserException")) 
      << call;
    C.zeroIndent();
    C << nl << "#endif";
    C.restoreIndent();
}

bool
Slice::Gen::setUseWstring(ContainedPtr p, list<bool>& hist, bool use)
{
    hist.push_back(use);
    StringList metaData = p->getMetaData();
    if(find(metaData.begin(), metaData.end(), "cpp:type:wstring") != metaData.end())
    {
        use = true;
    }
    else if(find(metaData.begin(), metaData.end(), "cpp:type:string") != metaData.end())
    {
        use = false;
    }
    return use;
}

bool
Slice::Gen::resetUseWstring(list<bool>& hist)
{
    bool use = hist.back();
    hist.pop_back();
    return use;
}
