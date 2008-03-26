# Confirm that boost library is installed

# This module defines
# BOOST_GRAPH_FOUND, If false, don't try to use Boost Graph Library

CHECK_INCLUDE_FILE_CXX( boost/graph/adjacency_list.hpp BOOST_GRAPH_FOUND )

IF( BOOST_GRAPH_FOUND )
    MESSAGE( STATUS "Looking for Boost Graph - found")
ELSE ( BOOST_GRAPH_FOUND )
    MESSAGE( STATUS "Looking for Boost Graph - not found")
ENDIF( BOOST_GRAPH_FOUND )
