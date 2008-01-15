/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
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

Slice::Gen::Gen(const string& name, const string& base, const string& headerExtension,
                const string& sourceExtension, const vector<string>& extraHeaders, const string& include,
                const vector<string>& includePaths, const string& dllExport, const string& dir,
                bool imp, bool checksum, bool stream, bool ice) :
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
    _ice(ice)
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

    printHeader(H);
    printHeader(C);
    H << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";
    C << "\n// Generated from file `" << changeInclude(_base, _includePaths) << ".ice'\n";

    string s = fileH;
    if(_include.size())
    {
        s = _include + '/' + s;
    }
    transform(s.begin(), s.end(), s.begin(), ToIfdef());
    H << "\n#ifndef __orca_strings_" << s << "__";
    H << "\n#define __orca_strings_" << s << "__";
    H << '\n';
}

Slice::Gen::~Gen()
{
    H << nl << eb;
    H << "\n\n#endif\n";
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
//     writeExtraHeaders(C);

    C << "\n#include <";
    if(_include.size())
    {
        C << _include << '/';
    }
    C << _base << "." << _headerExtension << ">";

    H << "\n#include <string>";
    H << "\n#include <XXX/"<< _base << "." << _headerExtension << ">";

    H << "\n\nnamespace orcaobjutil";
    H << sb << nl;

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
// 
//     string name = fixKwd(p->name());
//     
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
//     if(!p->isLocal())
//     {
//         string name = fixKwd(p->name());
//         string scope = fixKwd(p->scope());
// 
//         C << sp << nl << "void";
//         C << nl << scope.substr(2) << "__addObject(const " << name << "Ptr& p, ::IceInternal::GCCountMap& c)";
//         C << sb;
//         C << nl << "p->__addObject(c);";
//         C << eb;
// 
//         C << sp << nl << "bool";
//         C << nl << scope.substr(2) << "__usesClasses(const " << name << "Ptr& p)";
//         C << sb;
//         C << nl << "return p->__usesClasses();";
//         C << eb;
// 
//         C << sp << nl << "void";
//         C << nl << scope.substr(2) << "__decRefUnsafe(const " << name << "Ptr& p)";
//         C << sb;
//         C << nl << "p->__decRefUnsafe();";
//         C << eb;
// 
//         C << sp << nl << "void";
//         C << nl << scope.substr(2) << "__clearHandleUnsafe(" << name << "Ptr& p)";
//         C << sb;
//         C << nl << "p.__clearHandleUnsafe();";
//         C << eb;
//     }
    return false;
}

bool
Slice::Gen::TypesVisitor::visitExceptionStart(const ExceptionPtr& p)
{
    return true;
}

void
Slice::Gen::TypesVisitor::visitExceptionEnd(const ExceptionPtr& p)
{
}

bool
Slice::Gen::TypesVisitor::visitStructStart(const StructPtr& p)
{
    _useWstring = setUseWstring(p, _useWstringHist, _useWstring);

    string name = fixKwd(p->name());
    string scope = fixKwd(p->scope());

//     H << sp << nl << "struct " << name;
//     H << sb;
    
    H << "\nstd::string toString( const " << scope.substr(2) << name << "& );";

    C << "\n\nstd::string";
    C << nl << scope.substr(2) << "toString( const " << scope.substr(2) << name << "& obj )";
    C << sb;
    C << nl << "std::ostringstream s;";
    C << nl << "s <<";

//     H << "\nvoid init( " << scope.substr(2) << name << "& );";

//     C << "\n\nvoid";
//     C << nl << scope.substr(2) << "init( " << scope.substr(2) << name << "& obj )";
//     C << sb;
//     C << nl << "std::ostringstream s;";
//     C << nl << "s <<";

    return true;
}

void
Slice::Gen::TypesVisitor::visitStructEnd(const StructPtr& p)
{
//     H << eb << ';';
        
    C << ";";
    C << "\n    return s.str();";
    C << eb;

    _useWstring = resetUseWstring(_useWstringHist);
}

void
Slice::Gen::TypesVisitor::visitDataMember(const DataMemberPtr& p)
{
    string name = fixKwd(p->name());
//     string s = typeToString(p->type(), _useWstring, p->getMetaData());
//     H << nl << s << ' ' << name << ';';

    C << "\n      << \"" << name << " = \" << toString( obj." << name << " )";
}

void
Slice::Gen::TypesVisitor::visitSequence(const SequencePtr& p)
{
    string name = fixKwd(p->name());
    TypePtr type = p->type();
    string s = typeToString(type, _useWstring, p->typeMetaData());
    StringList metaData = p->getMetaData();
    string seqType = findMetaData(metaData, false);
    if(!seqType.empty())
    {
        H << sp << nl << "typedef " << seqType << ' ' << name << ';';
    }
    else
    {
        H << sp << nl << "typedef ::std::vector<" << (s[0] == ':' ? " " : "") << s << "> " << name << ';';
    }

    BuiltinPtr builtin = BuiltinPtr::dynamicCast(type);
    if(!p->isLocal())
    {
        string scoped = fixKwd(p->scoped());
        string scope = fixKwd(p->scope());

        H << sp << nl << "class __U__" << name << " { };";

        if(!seqType.empty())
        {
            H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << name << "&, __U__"
              << name << ");";
            H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "&, __U__" << name << ");";

            if(_stream)
            {
                H << nl << _dllExport << "void ice_write" << p->name() << "(const ::Ice::OutputStreamPtr&, const "
                  << name << "&);";
                H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name 
                  << "&);";
            }

            C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
              << scoped << "& v, " << scope << "__U__" << name << ")";
            C << sb;
            C << nl << "::Ice::Int size = static_cast< ::Ice::Int>(v.size());";
            C << nl << "__os->writeSize(size);";
            C << nl << "for(" << name << "::const_iterator p = v.begin(); p != v.end(); ++p)";
            C << sb;
            writeMarshalUnmarshalCode(C, type, "(*p)", true);
            C << eb;
            C << eb;

            C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
              << "& v, " << scope << "__U__" << name << ')';
            C << sb;
            C << nl << "::Ice::Int sz;";
            C << nl << "__is->readSize(sz);";
            C << nl << name << "(sz).swap(v);";
            if(type->isVariableLength())
            {
                // Protect against bogus sequence sizes.
                C << nl << "__is->startSeq(sz, " << type->minWireSize() << ");";
            }
            else
            {
                C << nl << "__is->checkFixedSeq(sz, " << type->minWireSize() << ");";
            }
            C << nl << "for(" << name << "::iterator p = v.begin(); p != v.end(); ++p)";
            C << sb;
            writeMarshalUnmarshalCode(C, type, "(*p)", false);

            //
            // After unmarshaling each element, check that there are still enough bytes left in the stream
            // to unmarshal the remainder of the sequence, and decrement the count of elements
            // yet to be unmarshaled for sequences with variable-length element type (that is, for sequences
            // of classes, structs, dictionaries, sequences, strings, or proxies). This allows us to
            // abort unmarshaling for bogus sequence sizes at the earliest possible moment.
            // (For fixed-length sequences, we don't need to do this because the prediction of how many
            // bytes will be taken up by the sequence is accurate.)
            //
            if(type->isVariableLength())
            {
                if(!SequencePtr::dynamicCast(type))
                {
                    //
                    // No need to check for directly nested sequences because, at the start of each
                    // sequence, we check anyway.
                    //
                    C << nl << "__is->checkSeq();";
                }
                C << nl << "__is->endElement();";
            }
            C << eb;
            if(type->isVariableLength())
            {
                C << nl << "__is->endSeq(sz);";
            }
            C << eb;

            if(_stream)
            {
                C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
                  << "(const ::Ice::OutputStreamPtr& __outS, const " << scoped << "& v)";
                C << sb;
                C << nl << "__outS->writeSize(::Ice::Int(v.size()));";
                C << nl << scoped << "::const_iterator p;";
                C << nl << "for(p = v.begin(); p != v.end(); ++p)";
                C << sb;
                writeStreamMarshalUnmarshalCode(C, type, "(*p)", true, "", _useWstring);
                C << eb;
                C << eb;

                C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
                  << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
                C << sb;
                C << nl << "::Ice::Int sz = __inS->readSize();";
                C << nl << scoped << "(sz).swap(v);";
                C << nl << scoped << "::iterator p;";
                C << nl << "for(p = v.begin(); p != v.end(); ++p)";
                C << sb;
                writeStreamMarshalUnmarshalCode(C, type, "(*p)", false, "", _useWstring);
                C << eb;
                C << eb;
            }
        }
        else if(!builtin || builtin->kind() == Builtin::KindObject || builtin->kind() == Builtin::KindObjectProxy)
        {
            H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << s << "*, const " << s
              << "*, __U__" << name << ");";
            H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "&, __U__" << name << ");";

            if(_stream)
            {
                H << nl << _dllExport << "void ice_write" << p->name() << "(const ::Ice::OutputStreamPtr&, const "
                  << name << "&);";
                H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name 
                  << "&);";
            }

            C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
              << s << "* begin, const " << s << "* end, " << scope << "__U__" << name << ")";
            C << sb;
            C << nl << "::Ice::Int size = static_cast< ::Ice::Int>(end - begin);";
            C << nl << "__os->writeSize(size);";
            C << nl << "for(int i = 0; i < size; ++i)";
            C << sb;
            writeMarshalUnmarshalCode(C, type, "begin[i]", true);
            C << eb;
            C << eb;

            C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
              << "& v, " << scope << "__U__" << name << ')';
            C << sb;
            C << nl << "::Ice::Int sz;";
            C << nl << "__is->readSize(sz);";
            if(type->isVariableLength())
            {
                // Protect against bogus sequence sizes.
                C << nl << "__is->startSeq(sz, " << type->minWireSize() << ");";
            }
            else
            {
                C << nl << "__is->checkFixedSeq(sz, " << type->minWireSize() << ");";
            }
            C << nl << "v.resize(sz);";
            C << nl << "for(int i = 0; i < sz; ++i)";
            C << sb;
            writeMarshalUnmarshalCode(C, type, "v[i]", false);

            //
            // After unmarshaling each element, check that there are still enough bytes left in the stream
            // to unmarshal the remainder of the sequence, and decrement the count of elements
            // yet to be unmarshaled for sequences with variable-length element type (that is, for sequences
            // of classes, structs, dictionaries, sequences, strings, or proxies). This allows us to
            // abort unmarshaling for bogus sequence sizes at the earliest possible moment.
            // (For fixed-length sequences, we don't need to do this because the prediction of how many
            // bytes will be taken up by the sequence is accurate.)
            //
            if(type->isVariableLength())
            {
                if(!SequencePtr::dynamicCast(type))
                {
                    //
                    // No need to check for directly nested sequences because, at the start of each
                    // sequence, we check anyway.
                    //
                    C << nl << "__is->checkSeq();";
                }
                C << nl << "__is->endElement();";
            }
            C << eb;
            if(type->isVariableLength())
            {
                C << nl << "__is->endSeq(sz);";
            }
            C << eb;

            if(_stream)
            {
                C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
                  << "(const ::Ice::OutputStreamPtr& __outS, const " << scoped << "& v)";
                C << sb;
                C << nl << "__outS->writeSize(::Ice::Int(v.size()));";
                C << nl << scoped << "::const_iterator p;";
                C << nl << "for(p = v.begin(); p != v.end(); ++p)";
                C << sb;
                writeStreamMarshalUnmarshalCode(C, type, "(*p)", true, "", _useWstring);
                C << eb;
                C << eb;

                C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
                  << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
                C << sb;
                C << nl << "::Ice::Int sz = __inS->readSize();";
                C << nl << "v.resize(sz);";
                C << nl << "for(int i = 0; i < sz; ++i)";
                C << sb;
                writeStreamMarshalUnmarshalCode(C, type, "v[i]", false, "", _useWstring);
                C << eb;
                C << eb;
            }
        }
    }
}

void
Slice::Gen::TypesVisitor::visitDictionary(const DictionaryPtr& p)
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
    H << sp << nl << "typedef ::std::map<" << ks << ", " << vs << "> " << name << ';';

    if(!p->isLocal())
    {
        string scoped = fixKwd(p->scoped());
        string scope = fixKwd(p->scope());

        H << sp << nl << "class __U__" << name << " { };";
        H << nl << _dllExport << "void __write(::IceInternal::BasicStream*, const " << name
          << "&, __U__" << name << ");";
        H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name
          << "&, __U__" << name << ");";

        if(_stream)
        {
            H << nl << _dllExport << "void ice_write" << p->name() << "(const ::Ice::OutputStreamPtr&, const " << name
              << "&);";
            H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name << "&);";
        }

        C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, const "
          << scoped << "& v, " << scope << "__U__" << name << ")";
        C << sb;
        C << nl << "__os->writeSize(::Ice::Int(v.size()));";
        C << nl << scoped << "::const_iterator p;";
        C << nl << "for(p = v.begin(); p != v.end(); ++p)";
        C << sb;
        writeMarshalUnmarshalCode(C, keyType, "p->first", true);
        writeMarshalUnmarshalCode(C, valueType, "p->second", true);
        C << eb;
        C << eb;

        C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
          << "& v, " << scope << "__U__" << name << ')';
        C << sb;
        C << nl << "::Ice::Int sz;";
        C << nl << "__is->readSize(sz);";
        C << nl << "while(sz--)";
        C << sb;
        C << nl << "::std::pair<const " << ks << ", " << vs << "> pair;";
        const string pf = string("const_cast<") + ks + "&>(pair.first)";
        writeMarshalUnmarshalCode(C, keyType, pf, false);
        C << nl << scoped << "::iterator __i = v.insert(v.end(), pair);";
        writeMarshalUnmarshalCode(C, valueType, "__i->second", false);
        C << eb;
        C << eb;

        if(_stream)
        {
            C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
              << "(const ::Ice::OutputStreamPtr& __outS, const " << scoped << "& v)";
            C << sb;
            C << nl << "__outS->writeSize(::Ice::Int(v.size()));";
            C << nl << scoped << "::const_iterator p;";
            C << nl << "for(p = v.begin(); p != v.end(); ++p)";
            C << sb;
            writeStreamMarshalUnmarshalCode(C, keyType, "p->first", true, "", _useWstring, p->keyMetaData());
            writeStreamMarshalUnmarshalCode(C, valueType, "p->second", true, "", _useWstring, p->valueMetaData());
            C << eb;
            C << eb;

            C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
              << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
            C << sb;
            C << nl << "::Ice::Int sz = __inS->readSize();";
            C << nl << "while(sz--)";
            C << sb;
            C << nl << "::std::pair<const " << ks << ", " << vs << "> pair;";
            writeStreamMarshalUnmarshalCode(C, keyType, pf, false, "", _useWstring, p->keyMetaData());
            C << nl << scoped << "::iterator __i = v.insert(v.end(), pair);";
            writeStreamMarshalUnmarshalCode(C, valueType, "__i->second", false, "", _useWstring, p->valueMetaData());
            C << eb;
            C << eb;
        }
    }
}

void
Slice::Gen::TypesVisitor::visitEnum(const EnumPtr& p)
{
    string name = fixKwd(p->name());
    EnumeratorList enumerators = p->getEnumerators();
    H << sp << nl << "enum " << name;
    H << sb;
    EnumeratorList::const_iterator en = enumerators.begin();
    while(en != enumerators.end())
    {
        H << nl << fixKwd((*en)->name());
        if(++en != enumerators.end())
        {
            H << ',';
        }
    }
    H << eb << ';';

    if(!p->isLocal())
    {
        string scoped = fixKwd(p->scoped());
        string scope = fixKwd(p->scope());
        
        size_t sz = enumerators.size();
        assert(sz <= 0x7fffffff); // 64-bit enums are not supported
        
        H << sp << nl << _dllExport << "void __write(::IceInternal::BasicStream*, " << name << ");";
        H << nl << _dllExport << "void __read(::IceInternal::BasicStream*, " << name << "&);";

        if(_stream)
        {
            H << sp << nl << _dllExport << "void ice_write" << p->name()
              << "(const ::Ice::OutputStreamPtr&, " << name << ");";
            H << nl << _dllExport << "void ice_read" << p->name() << "(const ::Ice::InputStreamPtr&, " << name << "&);";
        }

        C << sp << nl << "void" << nl << scope.substr(2) << "__write(::IceInternal::BasicStream* __os, " << scoped
          << " v)";
        C << sb;
        if(sz <= 0x7f)
        {
            C << nl << "__os->write(static_cast< ::Ice::Byte>(v));";
        }
        else if(sz <= 0x7fff)
        {
            C << nl << "__os->write(static_cast< ::Ice::Short>(v));";
        }
        else
        {
            C << nl << "__os->write(static_cast< ::Ice::Int>(v));";
        }
        C << eb;

        C << sp << nl << "void" << nl << scope.substr(2) << "__read(::IceInternal::BasicStream* __is, " << scoped
          << "& v)";
        C << sb;
        if(sz <= 0x7f)
        {
            C << nl << "::Ice::Byte val;";
            C << nl << "__is->read(val);";
            C << nl << "v = static_cast< " << scoped << ">(val);";
        }
        else if(sz <= 0x7fff)
        {
            C << nl << "::Ice::Short val;";
            C << nl << "__is->read(val);";
            C << nl << "v = static_cast< " << scoped << ">(val);";
        }
        else
        {
            C << nl << "::Ice::Int val;";
            C << nl << "__is->read(val);";
            C << nl << "v = static_cast< " << scoped << ">(val);";
        }
        C << eb;

        if(_stream)
        {
            C << sp << nl << "void" << nl << scope.substr(2) << "ice_write" << p->name()
              << "(const ::Ice::OutputStreamPtr& __outS, " << scoped << " v)";
            C << sb;
            if(sz <= 0x7f)
            {
                C << nl << "__outS->writeByte(static_cast< ::Ice::Byte>(v));";
            }
            else if(sz <= 0x7fff)
            {
                C << nl << "__outS->writeShort(static_cast< ::Ice::Short>(v));";
            }
            else
            {
                C << nl << "__outS->writeInt(static_cast< ::Ice::Int>(v));";
            }
            C << eb;

            C << sp << nl << "void" << nl << scope.substr(2) << "ice_read" << p->name()
              << "(const ::Ice::InputStreamPtr& __inS, " << scoped << "& v)";
            C << sb;
            if(sz <= 0x7f)
            {
                C << nl << "::Ice::Byte val = __inS->readByte();";
                C << nl << "v = static_cast< " << scoped << ">(val);";
            }
            else if(sz <= 0x7fff)
            {
                C << nl << "::Ice::Short val = __inS->readShort();";
                C << nl << "v = static_cast< " << scoped << ">(val);";
            }
            else
            {
                C << nl << "::Ice::Int val = __inS->readInt();";
                C << nl << "v = static_cast< " << scoped << ">(val);";
            }
            C << eb;
        }
    }
}

void
Slice::Gen::TypesVisitor::visitConst(const ConstPtr& p)
{
    H << sp;
    H << nl << "const " << typeToString(p->type(), _useWstring, p->typeMetaData()) << " " << fixKwd(p->name()) 
      << " = ";

    BuiltinPtr bp = BuiltinPtr::dynamicCast(p->type());
    if(bp && bp->kind() == Builtin::KindString)
    {
        //
        // Expand strings into the basic source character set. We can't use isalpha() and the like
        // here because they are sensitive to the current locale.
        //
        static const string basicSourceChars = "abcdefghijklmnopqrstuvwxyz"
                                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                               "0123456789"
                                               "_{}[]#()<>%:;.?*+-/^&|~!=,\\\"' ";
        static const set<char> charSet(basicSourceChars.begin(), basicSourceChars.end());

        if(_useWstring || findMetaData(p->typeMetaData(), true) == "wstring")
        {
            H << 'L';
        }
        H << "\"";                                      // Opening "

        const string val = p->value();
        for(string::const_iterator c = val.begin(); c != val.end(); ++c)
        {
            if(charSet.find(*c) == charSet.end())
            {
                unsigned char uc = *c;                  // char may be signed, so make it positive
                ostringstream s;
                s << "\\";                              // Print as octal if not in basic source character set
                s.width(3);
                s.fill('0');
                s << oct;
                s << static_cast<unsigned>(uc);
                H << s.str();
            }
            else
            {
                H << *c;                                // Print normally if in basic source character set
            }
        }

        H << "\"";                                      // Closing "
    }
    else if(bp && bp->kind() == Builtin::KindLong)
    {
        H << "ICE_INT64(" << p->value() << ")";
    }
    else
    {
        EnumPtr ep = EnumPtr::dynamicCast(p->type());
        if(ep)
        {
            H << fixKwd(p->value());
        }
        else
        {
            H << p->value();
        }
    }

    H << ';';
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
