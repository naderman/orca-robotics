#
# define the project version so we can have access to it from the code
#

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

# alexm: for gcc need to produce this in the Makefile: -DORCA_VERSION=\"X.Y.Z\", 
#        without escaping the quotes the compiler will strip them off.
# alexb: it seems that you also need to escape the quotes for windoze??
add_definitions( "-DPROJECT_VERSION=\\\"${GBX_PROJECT_VERSION}\\\"" )
add_definitions( "-DCMAKE_INSTALL_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"" )
