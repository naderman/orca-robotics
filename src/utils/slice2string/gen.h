/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef GEN_H
#define GEN_H

#include <Slice/Parser.h>
#include <IceUtil/OutputUtil.h>

namespace slice2string
{

class Gen : private ::IceUtil::noncopyable
{
public:

    Gen(const std::string&,
        const std::string&,
        const std::string&,
        const std::string&,
        const std::vector<std::string>&,
        const std::string&,
        const std::vector<std::string>&,
        const std::string&,
        const std::string&,
        bool,
        bool,
        bool,
        bool,
        // custom option
        const std::string& module);
    ~Gen();

    bool operator!() const; // Returns true if there was a constructor error

    void generate(const Slice::UnitPtr&);
    
    static bool setUseWstring(Slice::ContainedPtr, std::list<bool>&, bool);
    static bool resetUseWstring(std::list<bool>&);

private:

#ifdef ICE_VERSION_33
    void writeExtraHeaders(::IceUtilInternal::Output&);

    ::IceUtilInternal::Output H;
    ::IceUtilInternal::Output C;
#else
    void writeExtraHeaders(::IceUtil::Output&);

    ::IceUtil::Output H;
    ::IceUtil::Output C;
#endif

    std::string _base;
    std::string _headerExtension;
    std::string _sourceExtension;
    std::vector<std::string> _extraHeaders;
    std::string _include;
    std::vector<std::string> _includePaths;
    std::string _dllExport;
    bool _impl;
    bool _checksum;
    bool _stream;
    bool _ice;
    // custom option
    std::string _module;

    class GlobalIncludeVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

#ifdef ICE_VERSION_33
        GlobalIncludeVisitor(::IceUtilInternal::Output&);
#else
        GlobalIncludeVisitor(::IceUtil::Output&);
#endif

        virtual bool visitModuleStart(const Slice::ModulePtr&);

    private:

#ifdef ICE_VERSION_33
        ::IceUtilInternal::Output& H;
#else
        ::IceUtil::Output& H;
#endif

        bool _finished;
    };

    class TypesVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

#ifdef ICE_VERSION_33
        TypesVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, const std::string&, bool);
#else
        TypesVisitor(::IceUtil::Output&, ::IceUtil::Output&, const std::string&, bool);
#endif

        virtual bool visitModuleStart(const Slice::ModulePtr&);
        virtual void visitModuleEnd(const Slice::ModulePtr&);
        virtual bool visitClassDefStart(const Slice::ClassDefPtr&);
        virtual void visitClassDefEnd(const Slice::ClassDefPtr&);
        virtual bool visitExceptionStart(const Slice::ExceptionPtr&);
        virtual void visitExceptionEnd(const Slice::ExceptionPtr&);
        virtual bool visitStructStart(const Slice::StructPtr&);
        virtual void visitStructEnd(const Slice::StructPtr&);
        virtual void visitSequence(const Slice::SequencePtr&);
        virtual void visitDictionary(const Slice::DictionaryPtr&);
        virtual void visitEnum(const Slice::EnumPtr&);
        virtual void visitConst(const Slice::ConstPtr&);
        virtual void visitDataMember(const Slice::DataMemberPtr&);

    private:

        void emitUpcall(const Slice::ExceptionPtr&, const std::string&, bool = false);

#ifdef ICE_VERSION_33
        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;
#else
        ::IceUtil::Output& H;
        ::IceUtil::Output& C;
#endif

        std::string _dllExport;
        bool _stream;
        bool _doneStaticSymbol;
        bool _useWstring;
        std::list<bool> _useWstringHist;
    };
};

}

#endif
