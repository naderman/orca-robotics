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
using namespace slice2log;
using namespace Slice;
using namespace IceUtilInternal;

namespace detail {

void printHeader( ::IceUtilInternal::Output& out )
{
    out << "/*";
    out << "\n * Auto-generated by slice2log.";
    out << "\n * Do not modify or commit to source repository.";
    out << "\n */";
    out << "\n";
}

}

Gen::Gen(const string& name, const string& base, const string& headerExtension,
                const string& sourceExtension, const vector<string>& extraHeaders, const string& include,
                const vector<string>& includePaths, const string& dllExport, const string& dir,
                bool imp, bool checksum, bool stream, bool ice,
        // custom options
        const std::string& module, bool genString, bool log, bool init) :
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
    _module(module),
    _string(genString),
    _log(log),
    _init(init)
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
    H << "\n// Generated from file `" << _base << ".ice'\n";
    C << "\n// Generated from file `" << _base << ".ice'\n";

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

Gen::~Gen()
{
//     cout<<"DEBUG: Gen::~Gen()"<<endl;
//     H << nl << eb;
    H << "\n\n}";
    H << "\n\n#endif";
    H << '\n';

    C << '\n';
    C << "\n}";
    C << '\n';
}

bool
Gen::operator!() const
{
    if(!H || !C)
    {
        return true;
    }
    return false;
}

void
Gen::generate(const UnitPtr& p)
{    
//     cout<<"DEBUG: Gen::generate()"<<endl;
    writeExtraHeaders(C);

    string base_namespace;
    if ( _string )
    {
        base_namespace = "ifacestring";
    }
    else if ( _log )
    {
        base_namespace = "ifacelog";
    }
//     string lib_namespace = _module + base_namespace;
    string lib_namespace = base_namespace;
    string lib_dir = _module + lib_namespace;

    if ( _string )
    {
        H << "\n#include <string>";
    }
    if ( _log )
    {
        H << "\n#include <fstream>";
    }

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
        string::size_type pos_mod = include_module.find_last_of("/\\");
        if(pos_mod != string::npos)
        {
            // remove base
            include_module.erase(pos_mod, string::npos);
            string::size_type pos_dir = include_module.find_last_of("/\\");
            if(pos_dir != string::npos) {
                // remove dir
                include_module.erase(0, pos_dir+1);
            }
        }    
        string include_namespace = include_module + base_namespace;

        H << "\n#include <" << include_namespace << "/" << include_base << "." << _headerExtension << ">";
    }
    H << "\n";
    H << "\nnamespace " << lib_namespace;
    H << "\n{";
    H << "\n";

    C << "\n#include <" << lib_dir << "/" << _base << ".h>";
    C << "\n#include <gbxutilacfr/exceptions.h>";
    C << "\n#include \"util.h\"";
    C << "\n";
    C << "\nusing namespace std;";
    C << "\n";
    C << "\nnamespace " << lib_namespace;
    C << "\n{";

    if ( _string )
    {
        ToStringVisitor toStringVisitor(H, C, _dllExport, _stream);
        p->visit(&toStringVisitor, false);
    }
    if ( _log )
    {
        // only used to define bogus Prx functions
        HandleVisitor handleVisitor(H, C, _dllExport, _stream);
        handleVisitor.libNamespace_ = lib_namespace;
        p->visit(&handleVisitor, false);
    
        ToLogVisitor toLogVisitor(H, C, _dllExport, _stream);
        p->visit(&toLogVisitor, false);
    
        H << "\n";
    
        FromLogVisitor fromLogVisitor(H, C, _dllExport, _stream);
        p->visit(&fromLogVisitor, false);
    }
}

void
Gen::writeExtraHeaders(IceUtilInternal::Output& out)
{
//     cout<<"DEBUG: Gen::writeExtraHeaders()"<<endl;
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


Gen::ToStringVisitor::ToStringVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream), _doneStaticSymbol(false), _useWstring(false)
{
}

bool
Gen::ToStringVisitor::visitModuleStart(const ModulePtr& p)
{
//     cout<<"DEBUG: Gen::visitModuleStart()"<<endl;
//     _useWstring = setUseWstring(p, _useWstringHist, _useWstring);
//     H << sp << nl << "namespace " << name << nl << '{';
    return true;
}

void
Gen::ToStringVisitor::visitModuleEnd(const ModulePtr& p)
{
//     H << sp << nl << '}';
//     _useWstring = resetUseWstring(_useWstringHist);
}

bool
Gen::ToStringVisitor::visitClassDefStart(const ClassDefPtr& p)
{
//     cout<<"DEBUG: Gen::visitClassDefStart()"<<endl;
    if ( p->declaration()->isInterface() )
        return false;

    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    ClassList bases = p->bases();
    
    H << "\nstd::string toString( const " << scope.substr(2)<<name << "Ptr& obj, int recurse=1000, int expand=-1, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2)<<name << "Ptr& objPtr, int recurse, int expand, int indent )";
    C << sb;
    C << nl << "string ind;";
    C << nl << "for ( int i=0; i<indent; ++i ) ind += ' ';";

    C << nl << "string s = \"class " << scope.substr(2)<<name << "\";";
    C << nl << "if ( recurse>0 )";
    C << sb;

    ClassList::const_iterator q = bases.begin();
    if(!bases.empty()) {
        int count = 0;
        while(q != bases.end())
        {
            C << nl << fixKwd((*q)->scoped()) << "Ptr base" << count << "Ptr = objPtr;";
            C << nl << "s += \'\\n\' + ind + \"base \" + toString( base" << count << "Ptr, recurse, expand, indent+2 );";
            ++q;
            ++count;
        }
    }

    if ( !p->dataMembers().empty() ) {
        C << nl;
        C << nl << scope.substr(2)<<name << "& obj = *objPtr;";
    }
    return true;
}

void
Gen::ToStringVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    C << eb;
    C << nl << "return s;";
    C << eb;
}

bool
Gen::ToStringVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    return false;
}

void
Gen::ToStringVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
}

void
Gen::ToStringVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    EnumeratorList enumerators = p->getEnumerators();

    H << nl << "std::string toString( const " << scope.substr(2)<<name << ", int recurse=1000, int expand=-1, int indent=0 );";

    C << nl;
    C << nl << "string";
    C << nl << "toString( const " << scope.substr(2)<<name << " obj, int recurse, int expand, int indent )";
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
    C << nl << "ss << \"Unknown case in enumerator " << scope.substr(2)<<name << ": \"<< ((int)obj);";
    C << nl << "throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );";

    C << eb;
}

bool
Gen::ToStringVisitor::visitStructStart(const StructPtr& p)
{
//     cout<<"DEBUG: Gen::visitStructStart()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    
    H << "\nstd::string toString( const " << scope.substr(2)<<name << "& obj, int recurse=1000, int expand=-1, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2)<<name << "& obj, int recurse, int expand, int indent )";
    C << sb;
    C << nl << "string ind;";
    C << nl << "for ( int i=0; i<indent; ++i ) ind += ' ';";
    C << nl << "string s = \"struct " << scope.substr(2)<<name << "\";";
    C << nl << "if ( recurse>0 )";
    C << sb;
    return true;
}

void
Gen::ToStringVisitor::visitStructEnd(const StructPtr& p)
{       
    C << eb;
    C << nl << "return s;";
    C << eb;
}

void
Gen::ToStringVisitor::visitDataMember(const DataMemberPtr& p)
{
//     cout<<"DEBUG: Gen::visitDataMember()"<<endl;
    string name = fixKwd(p->name());

    C << nl << "s += \'\\n\' + ind + \"" + name + " = \" + toString( obj." + name + ", recurse-1, expand, indent+2 );";
}

void
Gen::ToStringVisitor::visitSequence(const SequencePtr& p)
{    
//     cout<<"DEBUG: Gen::visitSequence()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

    H << nl << "std::string toString( const " << scope.substr(2)<<name << "& obj, int recurse=1000, int expand=-1, int indent=0 );";

    C << "\n\nstring";
    C << nl << "toString( const " << scope.substr(2)<<name << "& obj, int recurse, int expand, int indent )";
    C << sb;
    C << nl << "return seqToString< " << scope.substr(2)<<name << " >( obj, recurse-1, expand, indent+2 );";
    C << eb;
}

void
Gen::ToStringVisitor::visitDictionary(const DictionaryPtr& p)
{
//     cout<<"DEBUG: Gen::visitDictionary()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

    H << nl << "std::string toString( const " << scope.substr(2)<<name << "& obj, int recurse=1000, int expand=-1, int indent=0 );";

    C << nl;
    C << nl << "string";
    C << nl << "toString( const " << scope.substr(2)<<name << "& obj, int recurse, int expand, int indent )";
    C << sb;
    C << nl << "return dictToString< " << scope.substr(2)<<name << "," << scope.substr(2)<<name << "::const_iterator >( obj, recurse-1, expand, indent+2 );";
    C << eb;
}

void
Gen::ToStringVisitor::visitConst(const ConstPtr& p)
{
}

void
Gen::ToStringVisitor::emitUpcall(const ExceptionPtr& base, const string& call, bool isLocal)
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

Gen::ToLogVisitor::ToLogVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream), _doneStaticSymbol(false), _useWstring(false)
{
}

bool
Gen::ToLogVisitor::visitModuleStart(const ModulePtr& p)
{
//     cout<<"DEBUG: Gen::visitModuleStart()"<<endl;
//     _useWstring = setUseWstring(p, _useWstringHist, _useWstring);
//     H << sp << nl << "namespace " << name << nl << '{';
    return true;
}

void
Gen::ToLogVisitor::visitModuleEnd(const ModulePtr& p)
{
//     H << sp << nl << '}';
//     _useWstring = resetUseWstring(_useWstringHist);
}

bool
Gen::ToLogVisitor::visitClassDefStart(const ClassDefPtr& p)
{
//     cout<<"DEBUG: Gen::visitClassDefStart()"<<endl;
    if ( p->declaration()->isInterface() )
        return false;

    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    string scoped = fixKwd(p->scoped());
    ClassList bases = p->bases();
    
    H << "\nvoid toLogStream( const " << scope.substr(2)<<name << "Ptr& obj, std::ostream& os, bool fromDerived=false );";

    C << "\n\nvoid";
    C << nl << "toLogStream( const " << scoped << "Ptr& objPtr, std::ostream& os, bool fromDerived )";
    C << sb;
    C << nl << "if ( objPtr == 0 )";
    C << sb;
    C << nl << "throw gbxutilacfr::Exception( ERROR_INFO, \"(while logging class " << scoped << ") cannot log a null pointer\" );";
    C << eb;

    C << nl << "if ( !fromDerived )";
    C << sb;
    C << nl << "string mostDerivedType = objPtr->ice_id();";
    C << nl << "if ( mostDerivedType.empty() )";
    C << sb;
    C << nl << "throw gbxutilacfr::Exception( ERROR_INFO, \"(while logging class " << scoped << ") got empty type ID\" );";
    C << eb;
    C << nl << "if ( mostDerivedType == \"" << scoped << "\" )";
    C << sb;
    C << nl << "// we are the most derived class, log our type";
    C << nl << "cout << \"DEBUG: logging \" << mostDerivedType << endl;";
    C << nl << "toLogStream( string(\"" << scoped << "\"), os );";
    C << eb;
    C << nl << "else";
    C << sb;
    C << nl << "// redirect to most derived class, and wait for him to come down to the bases";
    C << nl << "to_downCast( mostDerivedType, objPtr, os );";
    C << nl << "return;";
    C << eb;
    C << eb;

    ClassList::const_iterator q = bases.begin();
    if(!bases.empty()) {
        int count = 0;
        while(q != bases.end())
        {
            C << nl << fixKwd((*q)->scoped()) << "Ptr base" << count << "Ptr = objPtr;";
            C << nl << "toLogStream( base" << count << "Ptr, os, true );";
            ++q;
            ++count;
        }
    }

    if ( !p->dataMembers().empty() ) {
        C << nl;
        C << nl << scoped << "& obj = *objPtr;";
    }
    return true;
}

void
Gen::ToLogVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    C << eb;
}

bool
Gen::ToLogVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    return false;
}

void
Gen::ToLogVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
}

void
Gen::ToLogVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    EnumeratorList enumerators = p->getEnumerators();

    H << nl << "void toLogStream( const " << scope.substr(2)<<name << " obj, std::ostream& os );";

    C << nl;
    C << nl << "void";
    C << nl << "toLogStream( const " << scope.substr(2)<<name << " obj, std::ostream& os )";
    C << sb;
    C << nl << "toLogStream( (Ice::Int)obj, os );";
    C << eb;
}

bool
Gen::ToLogVisitor::visitStructStart(const StructPtr& p)
{
//     cout<<"DEBUG: Gen::visitStructStart()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    
    H << "\nvoid toLogStream( const " << scope.substr(2)<<name << "& obj, std::ostream& os );";

    C << "\n\nvoid";
    C << nl << "toLogStream( const " << scope.substr(2)<<name << "& obj, std::ostream& os )";
    C << sb;
    return true;
}

void
Gen::ToLogVisitor::visitStructEnd(const StructPtr& p)
{       
    C << eb;
}

void
Gen::ToLogVisitor::visitDataMember(const DataMemberPtr& p)
{
//     cout<<"DEBUG: Gen::visitDataMember()"<<endl;
    string name = fixKwd(p->name());

    C << nl << "toLogStream( obj." + name + ", os );";
}

void
Gen::ToLogVisitor::visitSequence(const SequencePtr& p)
{    
//     cout<<"DEBUG: Gen::visitSequence()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

    H << nl << "void toLogStream( const " << scope.substr(2)<<name << "& obj, std::ostream& os );";

    C << "\n\nvoid";
    C << nl << "toLogStream( const " << scope.substr(2)<<name << "& obj, std::ostream& os )";
    C << sb;
    C << nl << "seqToLogStream< " << scope.substr(2)<<name << " >( obj, os );";
    C << eb;
}

void
Gen::ToLogVisitor::visitDictionary(const DictionaryPtr& p)
{
//     cout<<"DEBUG: Gen::visitDictionary()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

    H << nl << "void toLogStream( const " << scope.substr(2)<<name << "& obj, std::ostream& os );";

    C << nl;
    C << nl << "void";
    C << nl << "toLogStream( const " << scope.substr(2)<<name << "& obj, std::ostream& os )";
    C << sb;
    C << nl << "dictToLogStream< " << scope.substr(2)<<name << "," << scope.substr(2)<<name << "::const_iterator >( obj, os );";
    C << eb;
}

void
Gen::ToLogVisitor::visitConst(const ConstPtr& p)
{
}

void
Gen::ToLogVisitor::emitUpcall(const ExceptionPtr& base, const string& call, bool isLocal)
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

///////

Gen::FromLogVisitor::FromLogVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream), _doneStaticSymbol(false), _useWstring(false)
{
}

bool
Gen::FromLogVisitor::visitModuleStart(const ModulePtr& p)
{
//     cout<<"DEBUG: Gen::visitModuleStart()"<<endl;
//     _useWstring = setUseWstring(p, _useWstringHist, _useWstring);
//     H << sp << nl << "namespace " << name << nl << '{';
    return true;
}

void
Gen::FromLogVisitor::visitModuleEnd(const ModulePtr& p)
{
//     H << sp << nl << '}';
//     _useWstring = resetUseWstring(_useWstringHist);
}

bool
Gen::FromLogVisitor::visitClassDefStart(const ClassDefPtr& p)
{
//     cout<<"DEBUG: Gen::visitClassDefStart()"<<endl;
    if ( p->declaration()->isInterface() )
        return false;

    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    string scoped = fixKwd(p->scoped());
    ClassList bases = p->bases();
    
    H << "\nvoid fromLogStream( " << scope.substr(2)<<name << "Ptr& obj, std::istream& is, bool fromDerived=false, bool fromBase=false );";

    C << "\n\nvoid";
    C << nl << "fromLogStream( " << scoped << "Ptr& objPtr, std::istream& is, bool fromDerived, bool fromBase )";
    C << sb;

    C << nl << "if ( !fromDerived )";
    C << sb;
    C << nl << "bool iAmTheOne = fromBase;";
    C << nl << "string mostDerivedType;";
    C << nl << "if ( !iAmTheOne )";
    C << sb;
    C << nl << "fromLogStream( mostDerivedType, is );";
    C << nl << "if ( mostDerivedType.empty() )";
    C << sb;
    C << nl << "throw gbxutilacfr::Exception( ERROR_INFO, \"(while replaying class " << scoped << ") got empty type ID\" );";
    C << eb;
    C << nl << "// we are the most derived class";
    C << nl << "if ( mostDerivedType == \"" << scoped << "\" )";
    C << sb;
    C << nl << "cout << \"DEBUG: replaying \" << mostDerivedType << endl;";
    C << nl << "iAmTheOne = true;";
    C << eb;
    C << nl << "else";
    C << sb;
    C << nl << "cout << \"DEBUG: redirecting replay to \" << mostDerivedType << endl;";
    C << eb;
    C << eb;
    C << nl << "if ( iAmTheOne )";
    C << sb;
    C << nl << "// instantiate ourselves";
    C << nl << "cout << \"DEBUG: replaying " << scoped << "\" << endl;";
    C << nl << "objPtr = new " << scoped << ";";
    C << eb;
    C << nl << "else";
    C << sb;
    C << nl << "// redirect to most derived class, and wait for him to come down to the bases";
    C << nl << "from_downCast( mostDerivedType, objPtr, is );";
    C << nl << "return;";
    C << eb;
    C << eb;

    ClassList::const_iterator q = bases.begin();
    if(!bases.empty()) {
        int count = 0;
        while(q != bases.end())
        {
            C << nl << fixKwd((*q)->scoped()) << "Ptr base" << count << "Ptr = objPtr;";
            C << nl << "fromLogStream( base" << count << "Ptr, is, true );";
            ++q;
            ++count;
        }
    }

    if ( !p->dataMembers().empty() ) {
        C << nl;
        C << nl << scope.substr(2)<<name << "& obj = *objPtr;";
    }
    return true;
}

void
Gen::FromLogVisitor::visitClassDefEnd(const ClassDefPtr& p)
{
    C << eb;
}

bool
Gen::FromLogVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    return false;
}

void
Gen::FromLogVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
}

void
Gen::FromLogVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    EnumeratorList enumerators = p->getEnumerators();

    H << nl << "void fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is );";

    C << nl;
    C << nl << "void";
    C << nl << "fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is )";
    C << sb;
    C << nl << "Ice::Int i;";
    C << nl << "fromLogStream( i, is );";
    C << nl << "obj =  (" << scope.substr(2)<<name << ")i;";
    C << eb;
}

bool
Gen::FromLogVisitor::visitStructStart(const StructPtr& p)
{
//     cout<<"DEBUG: Gen::visitStructStart()"<<endl;
    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());
    
    H << "\nvoid fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is );";

    C << "\n\nvoid";
    C << nl << "fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is )";
    C << sb;
    return true;
}

void
Gen::FromLogVisitor::visitStructEnd(const StructPtr& p)
{       
    C << eb;
}

void
Gen::FromLogVisitor::visitDataMember(const DataMemberPtr& p)
{
//     cout<<"DEBUG: Gen::visitDataMember()"<<endl;
    string name = fixKwd(p->name());

    C << nl << "fromLogStream( obj." + name + ", is );";
}

void
Gen::FromLogVisitor::visitSequence(const SequencePtr& p)
{    
//     cout<<"DEBUG: Gen::visitSequence()"<<endl;
    string name = fixKwd(p->name());

    TypePtr type = p->type();
    string s = typeToString(type, _useWstring, p->typeMetaData());
    StringList metaData = p->getMetaData();
    string seqType = findMetaData(metaData, false);
    string es;
    if(!seqType.empty())
    {
        es = seqType;
    }
    else
    {
        es = (s[0] == ':' ? " " : "") + s;
    }

    string scope = fixKwd(p->scope());

    H << nl << "void fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is );";

    C << "\n\nvoid";
    C << nl << "fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is )";
    C << sb;
    C << nl << "seqFromLogStream<"<<es<<">( obj, is );";
    C << eb;
}

void
Gen::FromLogVisitor::visitDictionary(const DictionaryPtr& p)
{
    string name = fixKwd(p->name());

    TypePtr keyType = p->keyType();
    TypePtr valueType = p->valueType();
    string ks = typeToString(keyType, _useWstring, p->keyMetaData());
    if(ks[0] == ':')
    {
        ks.insert(0, " ");
    }
    string vs = typeToString(valueType, _useWstring, p->valueMetaData());

    string scope = fixKwd(p->scope());

    H << nl << "void fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is );";

    C << nl;
    C << nl << "void";
    C << nl << "fromLogStream( " << scope.substr(2)<<name << "& obj, std::istream& is )";
    C << sb;
    C << nl << "dictFromLogStream<"<<ks<<","<<vs<<">( obj, is );";
    C << eb;
}

void
Gen::FromLogVisitor::visitConst(const ConstPtr& p)
{
}

void
Gen::FromLogVisitor::emitUpcall(const ExceptionPtr& base, const string& call, bool isLocal)
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

//////////

Gen::HandleVisitor::HandleVisitor(Output& h, Output& c, const string& dllExport, bool stream) :
    H(h), C(c), _dllExport(dllExport), _stream(stream)
{
}

bool
Gen::HandleVisitor::visitModuleStart(const ModulePtr& p)
{
    C << sp;
    C << nl << "namespace " << nl << '{';

    return true;
}

void
Gen::HandleVisitor::visitModuleEnd(const ModulePtr& p)
{
    // have to put into the main namespace because we are calling public functions from here

    string toFunctionName = "to_downCast";
    string fromFunctionName = "from_downCast";
    for ( InheritanceMap::const_iterator it=inherit_.begin(); it!=inherit_.end(); ++it ) {
        const string& base = it->first;
        const vector<string>& derives = it->second;

        // TO
        C << nl;
        C << nl << "void";
        C << nl << toFunctionName << "( const string& derivedType, const " << base << "Ptr& objPtr, std::ostream& os )";
        C << sb;
            C << nl << "// " << derives.size() << " derivatives";
            if ( derives.empty() ) {
                C << nl << "assert( false && \"should not get here\" );";
            }
    
            for ( size_t i=0; i<derives.size(); ++i ) {
                C << nl;
                if ( i==0 )
                    C << "if ";
                else
                    C << "else if ";
                C << "( derivedType == \"" << derives[i] << "\" )";
                C << sb;
                C << nl << derives[i] << "Ptr derivedPtr = " << derives[i] << "Ptr::dynamicCast( objPtr );";
                C << nl << libNamespace_<<"::toLogStream( derivedPtr, os );";
                C << eb;
            }
            if ( !derives.empty() ) {
                C << nl << "else";
                C << sb;
                C << nl << "assert( false && \"unknown derived class for " << base << "\" );";  
                C << eb;
            }
        C << eb;

        // FROM
        C << nl;
        C << nl << "void";
        C << nl << fromFunctionName << "( const string& derivedType, " << base << "Ptr& objPtr, std::istream& is )";
        C << sb;
            C << nl << "// " << derives.size() << " derivatives";
            if ( derives.empty() ) {
                C << nl << "assert( false && \"should not get here\" );";
            }
    
            for ( size_t i=0; i<derives.size(); ++i ) {
                C << nl;
                if ( i==0 )
                    C << "if ";
                else
                    C << "else if ";
                C << "( derivedType == \"" << derives[i] << "\" )";
                C << sb;
                C << nl << derives[i] << "Ptr derivedPtr = " << derives[i] << "Ptr::dynamicCast( objPtr );";
                C << nl << libNamespace_<<"::fromLogStream( derivedPtr, is, false, true );";
                C << nl << "objPtr = derivedPtr;";
                C << eb;
            }
            if ( !derives.empty() ) {
                C << nl << "else";
                C << sb;
                C << nl << "assert( false && \"unknown derived class for " << base << "\" );";  
                C << eb;
            }
        C << eb;
    }

    C << sp;
    C << nl << '}';
}

void
Gen::HandleVisitor::visitClassDecl(const ClassDeclPtr& p)
{
    string name = p->name();
    string scoped = fixKwd(p->scoped());

    if(!p->isLocal() && p->isInterface())
    {
        C << nl << "void toLogStream( const " << scoped << "Prx& prx, std::ostream& os )";
        C << sb;
        C << nl << "// this data is not logged.";
        C << eb;

        C << nl << "void fromLogStream( " << scoped << "Prx& prx, std::istream& is )";
        C << sb;
        C << nl << "// this data is not logged.";
        C << eb;
    }
}

bool
Gen::HandleVisitor::visitClassDefStart(const ClassDefPtr& p)
{
    if ( p->declaration()->isInterface() )
        return false;

    string derived = fixKwd(p->scoped());
    ClassList bases = p->bases();

    // make sure this (possibly) derived class is in the database
    // we want all classes to be here so we can define a function for each (possibly empty)
    InheritanceMap::iterator it = inherit_.find( derived );
    if ( it == inherit_.end() ) {
        vector<string> emptyVector;
        inherit_[derived] = emptyVector;
    }

    ClassList::const_iterator q = bases.begin();
    if(!bases.empty()) {
        while(q != bases.end())
        {
            string base = fixKwd((*q)->scoped());
            // make sure this base is in the database
            // cannot rely on the order of definition, because classes can be declared before
            InheritanceMap::iterator it = inherit_.find( base );
            if ( it == inherit_.end() ) {
                vector<string> emptyVector;
                inherit_[base] = emptyVector;
                // look for it again, now it should be here
                it = inherit_.find( base );
                assert( it != inherit_.end() && "twice failed to find base" );
            }
            it->second.push_back( derived );
            ++q;
        }
    }

    return true;
}

bool
Gen::setUseWstring(ContainedPtr p, list<bool>& hist, bool use)
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
Gen::resetUseWstring(list<bool>& hist)
{
    bool use = hist.back();
    hist.pop_back();
    return use;
}
