#
# Figure out if we are in the main Orca project or not
# If it is, all necessary files are in this project.
# Otherwise, we need to find Orca installation directory.
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
    IF ( DEFINED ORCA2_HOME )
        # Orca2 home is specified with a command line option or it's already in cache
        MESSAGE( STATUS "Orca2 location was specified or using cached value: ${ORCA2_HOME}" )
    ELSE ( DEFINED ORCA2_HOME )
        # Find Orca2 Installation
        # Will search several standard places starting with an env. variable ORCA2_HOME
        INCLUDE ( cmake/local/FindOrca2.cmake )

        ASSERT( ORCA2_FOUND 
            "Looking for Orca2 - not found. Please install Orca, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Orca2 - found in ${ORCA2_HOME}" 
            1 )

#         IF ( ORCA2_FOUND )    
#                 MESSAGE( STATUS "Looking for Orca2 - found in ${ORCA2_HOME}" )
#         ELSE ( ORCA2_FOUND )    
#                 MESSAGE( FATAL_ERROR "Looking for Orca2 - not found. Please install Orca2, ** delete CMakeCache.txt **, then re-run CMake." )    
#         ENDIF ( ORCA2_FOUND )

    ENDIF ( DEFINED ORCA2_HOME )

    SET ( ORCA_CMAKE_DIR ${ORCA2_HOME}/cmake CACHE PATH "Location of Orca CMake scripts" )

ENDIF ( ${PROJECT_NAME} MATCHES "orca" )

MESSAGE ( STATUS "Using custom CMake scripts in ${ORCA_CMAKE_DIR}" )

#
# The rest is done by a script
#
INCLUDE( ${ORCA_CMAKE_DIR}/global_setup.cmake )
