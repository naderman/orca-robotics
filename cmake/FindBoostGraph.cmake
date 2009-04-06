# Confirm that boost library is installed

# This module defines
# BOOST_GRAPH_FOUND, If false, don't try to use Boost Graph Library

check_include_file_cxx( boost/graph/adjacency_list.hpp BOOST_GRAPH_FOUND )

if( BOOST_GRAPH_FOUND )
    message( STATUS "Looking for Boost Graph - found")
else( BOOST_GRAPH_FOUND )
    message( STATUS "Looking for Boost Graph - not found")
endif( BOOST_GRAPH_FOUND )
