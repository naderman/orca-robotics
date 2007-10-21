#
# Figure out if we are in the main Orca project or not
# If it is, all necessary files are in this project.
# Otherwise, we need to find Orca installation directory.
#
# This file should be copied verbatim into [PROJECT]/cmake/local
# directory of satelite projects.
#
INCLUDE ( cmake/local/Assert.cmake )

IF ( ${PROJECT_NAME} MATCHES "orca" )
    MESSAGE ( STATUS "Setting project type to orca mothership" )
    SET ( ORCA_MOTHERSHIP 1 )

    SET ( ORCA_CMAKE_DIR ${${PROJECT_NAME}_SOURCE_DIR}/cmake CACHE PATH "Location of Orca CMake scripts" )

ELSE ( ${PROJECT_NAME} MATCHES "orca" )
    MESSAGE ( STATUS "Setting project type to orca satelite" )
    SET ( ORCA_MOTHERSHIP 0 )

    # If this is NOT the Orca project, we need to find Orca installation
    IF ( DEFINED ORCA_HOME )
        # Orca home is specified with a command line option or it's already in cache
        MESSAGE( STATUS "Orca location was specified or using cached value: ${ORCA_HOME}" )
    ELSE ( DEFINED ORCA_HOME )
        # Find Orca Installation
        # Will search several standard places starting with an env. variable ORCA_HOME
        INCLUDE ( cmake/local/FindOrca.cmake )

        ASSERT( ORCA_FOUND 
            "Looking for Orca - not found. Please install Orca, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Orca - found in ${ORCA_HOME}" 
            1 )

#         IF ( ORCA_FOUND )    
#                 MESSAGE( STATUS "Looking for Orca - found in ${ORCA_HOME}" )
#         ELSE ( ORCA_FOUND )    
#                 MESSAGE( FATAL_ERROR "Looking for Orca - not found. Please install Orca, ** delete CMakeCache.txt **, then re-run CMake." )    
#         ENDIF ( ORCA_FOUND )

    ENDIF ( DEFINED ORCA_HOME )

    SET ( ORCA_CMAKE_DIR ${ORCA_HOME}/cmake CACHE PATH "Location of Orca CMake scripts" )

ENDIF ( ${PROJECT_NAME} MATCHES "orca" )

MESSAGE ( STATUS "Using custom CMake scripts in ${ORCA_CMAKE_DIR}" )

#
# The rest is done by a script
#
INCLUDE( ${ORCA_CMAKE_DIR}/global_setup.cmake )
