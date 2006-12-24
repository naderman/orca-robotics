INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

# Glob all source files
FILE( GLOB HUMAN_SRCS *.cpp )

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/libs
)

SET( MOC_HEADERS
    guiqtdisplay.h
    mainwin.h
    probeview.h
    probedelegate.h
)

QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

ADD_LIBRARY( OrcaProbeGuiQt SHARED ${HUMAN_SRCS} ${MOC_SRCS} )

INSTALL_TARGETS( /lib OrcaProbeGuiQt )
