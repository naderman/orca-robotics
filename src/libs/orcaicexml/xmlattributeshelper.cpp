// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include "xmlattributeshelper.h"
#include <sstream>

using namespace std;

namespace orcaicexml
{

namespace {

//
// This is copy of a function toString()
// defined in [ICE-SRC]/src/IceGrid/Util.h
//
string toString(const vector<string>& v, const std::string& sep= std::string(" "))
{
    ostringstream os;
    vector<string>::const_iterator p = v.begin();
    while(p != v.end())
    {
        os << *p;
        ++p;
        if(p != v.end())
        {
            os << sep;
        }
    }
    return os.str();
}

} // namespace

XmlAttributesHelper::XmlAttributesHelper(const IceXML::Attributes& attrs, 
//                                          const Ice::LoggerPtr& logger,
                                         const string& filename,
                                         int line) :
    _attributes(attrs),
//     _logger(logger),
    _filename(filename),
    _line(line)
{
}

void
XmlAttributesHelper::checkUnknownAttributes()
{
    vector<string> notUsed;
    for(map<string, string>::const_iterator p = _attributes.begin(); p != _attributes.end(); ++p)
    {
        if(_used.find(p->first) == _used.end())
        {
            notUsed.push_back(p->first);
        }
    }

    if(!notUsed.empty())
    {
        ostringstream os;
        os << "unknown attributes in <" << _filename << "> descriptor, line " << _line << ":\n" << toString(notUsed);
        throw os.str();
    }
}

bool
XmlAttributesHelper::contains(const string& name) const
{
    _used.insert(name);
    return _attributes.find(name) != _attributes.end();
}

string 
XmlAttributesHelper::operator()(const string& name) const
{
    _used.insert(name);
    IceXML::Attributes::const_iterator p = _attributes.find(name);
    if(p == _attributes.end())
    {
        throw "missing attribute '" + name + "'";
    }
    string v = p->second;
    if(v.empty())
    {
        throw "attribute '" + name + "' is empty";
    }
    return v;
}

string
XmlAttributesHelper::operator()(const string& name, const string& def) const
{
    _used.insert(name);
    IceXML::Attributes::const_iterator p = _attributes.find(name);
    if(p == _attributes.end())
    {
        return def;
    }
    else
    {
        return p->second;
    }
}

map<string, string>
XmlAttributesHelper::asMap() const
{
    for(map<string, string>::const_iterator p = _attributes.begin(); p != _attributes.end(); ++p)
    {
        _used.insert(p->first);
    }
    return _attributes;
}

bool
XmlAttributesHelper::asBool(const string& name) const
{
    _used.insert(name);
    IceXML::Attributes::const_iterator p = _attributes.find(name);
    if(p == _attributes.end())
    {
        throw "missing attribute '" + name + "'";
        return true; // Keep the compiler happy.
    }
    else if(p->second == "true")
    {
        return true;
    }    
    else if(p->second == "false")
    {
        return false;
    }
    else
    {
        throw "invalid attribute `" + name + "': value is not 'false' or 'true'";
        return true; // Keep the compiler happy.
    }
}

bool
XmlAttributesHelper::asBool(const string& name, bool def) const
{
    _used.insert(name);
    IceXML::Attributes::const_iterator p = _attributes.find(name);
    if(p == _attributes.end())
    {
        return def;
    }
    else if(p->second == "true")
    {
        return true;
    }    
    else if(p->second == "false")
    {
        return false;
    }
    else
    {
        throw "invalid attribute `" + name + "': value is not 'false' or 'true'";
        return true; // Keep the compiler happy.
    }
}

} // namespace
