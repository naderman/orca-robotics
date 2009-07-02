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
#include <map>
#include <vector>

namespace slice2orca
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
        // custom options
        const std::string& module,
        bool,
        bool);
    ~Gen();

    bool operator!() const; // Returns true if there was a constructor error

    void generate(const Slice::UnitPtr&);
    
    static bool setUseWstring(Slice::ContainedPtr, std::list<bool>&, bool);
    static bool resetUseWstring(std::list<bool>&);

private:

    void writeExtraHeaders(::IceUtilInternal::Output&);

    ::IceUtilInternal::Output H;
    ::IceUtilInternal::Output C;

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
    // custom options
    std::string _module;
    bool _util;
    bool _log;

    class GlobalIncludeVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        GlobalIncludeVisitor(::IceUtilInternal::Output&);

        virtual bool visitModuleStart(const Slice::ModulePtr&);

    private:

        ::IceUtilInternal::Output& H;

        bool _finished;
    };

    class ToMetaDataVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        ToMetaDataVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&);

//         virtual bool visitClassDefStart(const Slice::ClassDefPtr&);
        virtual bool visitStructStart(const Slice::StructPtr&);

        // custom parameters
        std::string libDir_;
        std::string baseFilename_;
    private:

        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;
    };

    class ToStringVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        ToStringVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, const std::string&, bool);

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

        // custom parameter
        std::string libNamespace_;
        std::string libDir_;
        std::string baseFilename_;
    private:

        void emitUpcall(const Slice::ExceptionPtr&, const std::string&, bool = false);

        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;

        std::string _dllExport;
        bool _stream;
        bool _doneStaticSymbol;
        bool _useWstring;
        std::list<bool> _useWstringHist;
    };

    class ToInitVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        ToInitVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, const std::string&, bool);

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

        // alexm: the namespace of the generated library
        std::string libNamespace_;
    private:

        void emitUpcall(const Slice::ExceptionPtr&, const std::string&, bool = false);

        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;

        std::string _dllExport;
        bool _stream;
        bool _doneStaticSymbol;
        bool _useWstring;
        std::list<bool> _useWstringHist;
    };

    class ToLogVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        ToLogVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, const std::string&, bool);

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

        // alexm: the namespace of the generated library
        std::string libNamespace_;
    private:

        void emitUpcall(const Slice::ExceptionPtr&, const std::string&, bool = false);

        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;

        std::string _dllExport;
        bool _stream;
        bool _doneStaticSymbol;
        bool _useWstring;
        std::list<bool> _useWstringHist;
    };

    class FromLogVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        FromLogVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, const std::string&, bool);

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

        // alexm: the namespace of the generated library
        std::string libNamespace_;
    private:

        void emitUpcall(const Slice::ExceptionPtr&, const std::string&, bool = false);

        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;

        std::string _dllExport;
        bool _stream;
        bool _doneStaticSymbol;
        bool _useWstring;
        std::list<bool> _useWstringHist;
    };

    enum OutputType
    {
        OutputString,
        OutputInit,
        OutputToLog,
        OutputFromLog
    };

    class HandleVisitor : private ::IceUtil::noncopyable, public Slice::ParserVisitor
    {
    public:

        HandleVisitor(::IceUtilInternal::Output&, ::IceUtilInternal::Output&, const std::string&, bool, OutputType );

        virtual bool visitModuleStart(const Slice::ModulePtr&);
        virtual void visitModuleEnd(const Slice::ModulePtr&);
        virtual void visitClassDecl(const Slice::ClassDeclPtr&);
        virtual bool visitClassDefStart(const Slice::ClassDefPtr&);

        // alexm: the namespace of the generated library
        std::string libNamespace_;
    private:

        ::IceUtilInternal::Output& H;
        ::IceUtilInternal::Output& C;

        std::string _dllExport;
        bool _stream;

        // alexm: store inheritance information
        // e.g. Base <-- Derived <-- Super
        // as we go through derived classes we compile information for each base class
        // the map<string,vector<string> will contains the following
        // [B],<D,S>
        // [D],<S>
        typedef std::map<std::string,std::vector<std::string> > InheritanceMap;
        InheritanceMap inherit_;

        OutputType outputType_;
    };
};

}

#endif
