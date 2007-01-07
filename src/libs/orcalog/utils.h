/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_UTILS_H
#define ORCA2_ORCALOG_UTILS_H

#include <string>

namespace orcalog
{

//! Parse required interface tag into interface type and suffix.
//! The suffix is a number,e.g. '0', '1', '99', etc.
//! Raises orcalog::Exception if parsing error is encountered.
void parseRequiredTag( const std::string & tag, std::string & type, std::string & suffix );

//! Convert log parameters to a master file header line.
std::string headerLine( const std::string & filename, 
                        const std::string & interfaceType, 
                        const std::string & format );

//! Parse a master file header line to log parameters.
void parseHeaderLine( const std::string & line, 
                        std::string & filename, 
                        std::string & interfaceType, 
                        std::string & format );

//! Special string to mark the end of the header (start of data).
std::string endOfHeader();

//! Test master file line for special string marking the end of the header (start of data).
bool isEndOfHeader( const std::string & line );

//! Convert data parameters to a master file data line.
std::string dataLine( int seconds, int useconds, int id, int index );

//! Parse a master file data line to data parameters. Throws orcalog::Exception if the data line has
//! wrong format.
void parseDataLine( const std::string & line, int & seconds, int & useconds, int & id, int & index );

} //namespace

#endif
