#
# These utilities are not components.
# We do not include the standard component compile options
# to minimize dependencies.
#

IF ( NOT OS_WIN )

    # alexm: this is a mess, not sure what this one needs
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
#     ADD_DEFINITIONS( "-Wall" )
#     INCLUDE_DIRECTORIES( 
#         ${ICE_HOME}/include
#         ${PROJECT_SOURCE_DIR}/src/libs
#         )
#     LINK_DIRECTORIES( ${ICE_HOME}/lib )
    LINK_LIBRARIES( OrcaGpsUtil )

    GLOBAL_ADD_EXECUTABLE( latlon2mga latlon2mga.cpp )
    GLOBAL_ADD_EXECUTABLE( mga2latlon mga2latlon.cpp )

    # there is unexplained compile failure in qnx when using ostream
    IF ( OS_LINUX )
        GLOBAL_ADD_EXECUTABLE( latlon2mgafile latlon2mgafile.cpp )
    ENDIF ( OS_LINUX )	

ENDIF ( NOT OS_WIN )