/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAMISC_DIFF_H
#define ORCAMISC_DIFF_H

#include <string>

namespace orcamisc {

    //! Writes the outputString to a file named "test_output.txt",
    //! then compares the contents of that file to the contents
    //! of referenceFileName.  Returns true if it matches.
    bool outputMatchesReferenceFile( const std::string &outputString,
                                     const std::string &referenceFileName );

    bool filesAreSame( const std::string &filename1,
                       const std::string &filename2 );

}

#endif
