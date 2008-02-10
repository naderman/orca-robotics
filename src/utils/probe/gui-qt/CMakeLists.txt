GLOBAL_ADD_LICENSE( GPL )

INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

# Glob all source files
FILE( GLOB HUMAN_srcs *.cpp )

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/libs
)

SET( MOC_HEADERS
    guiqtdisplay.h
    mainwin.h
    probeview.h
    probedelegate.h
)

QT4_WRAP_CPP( MOC_srcs ${MOC_HEADERS} )

ADD_LIBRARY( OrcaProbeGuiQt SHARED ${HUMAN_srcs} ${MOC_srcs} )

INSTALL_TARGETS( /lib OrcaProbeGuiQt )
