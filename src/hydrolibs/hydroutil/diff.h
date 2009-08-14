/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROUTIL_DIFF_H
#define HYDROUTIL_DIFF_H

#include <string>

namespace hydroutil {

    //! Writes the outputString to a file named "test_output.txt",
    //! then compares the contents of that file to the contents
    //! of referenceFileName.  Returns true if it matches.
    bool outputMatchesReferenceFile( const std::string &outputString,
                                     const std::string &referenceFileName );

    bool filesAreSame( const std::string &filename1,
                       const std::string &filename2 );

}

#endif
