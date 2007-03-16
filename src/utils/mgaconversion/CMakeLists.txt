#
# These utilities are not components.
# We do not include the standard component compile options
# to minimize dependencies.
#

ADD_DEFINITIONS( "-Wall" )
INCLUDE_DIRECTORIES( 
	${ICE_HOME}/include
    ${PROJECT_SOURCE_DIR}/src/libs
    )
LINK_DIRECTORIES( ${ICE_HOME}/lib )
LINK_LIBRARIES( OrcaGpsUtil )

GLOBAL_ADD_EXECUTABLE( latlon2mga latlon2mga.cpp )

# there is unexplained compile failure in qnx when using ostream
IF ( OS_LINUX )
    GLOBAL_ADD_EXECUTABLE( latlon2mgafile latlon2mgafile.cpp )
ENDIF ( OS_LINUX )