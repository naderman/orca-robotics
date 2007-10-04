// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// This code is extracted from IceGrid
//

#ifndef ICE_XML_ATTRIBUTES_HELPER_H
#define ICE_XML_ATTRIBUTES_HELPER_H

#include <Ice/Logger.h>
#include <IceXML/Parser.h>
#include <set>

namespace orcaicexml
{

//! This code is extracted from IceGrid
class XmlAttributesHelper
{
public:
    //! Constructor
    XmlAttributesHelper(const IceXML::Attributes&, const Ice::LoggerPtr&, const std::string&, int);

    //! checkUnknownAttributes
    void checkUnknownAttributes();
    //! contains
    bool contains(const std::string&) const;
    //! asMap
    std::map<std::string, std::string> asMap() const;

    //! asBool
    bool asBool(const std::string&) const;
    //! asBool
    bool asBool(const std::string&, bool) const;

    //! lookup attribute
    std::string operator()(const std::string& attr) const;
    //! lookup attribute with default
    std::string operator()(const std::string& attr, const std::string& def) const;

private:
    
    const IceXML::Attributes& _attributes;
    const Ice::LoggerPtr _logger;
    const std::string _filename;
    const int _line;

    mutable std::set<std::string> _used;
};

}; // namespace

#endif
