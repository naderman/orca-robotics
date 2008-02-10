GLOBAL_ADD_LICENSE( GPL )

INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

# Glob all source files
FILE( GLOB human_srcs *.cpp )

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/libs
)

SET( moc_hdrs
    guiqtdisplay.h
    mainwin.h
    probeview.h
    probedelegate.h
)

QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

ADD_LIBRARY( OrcaProbeGuiQt SHARED ${human_srcs} ${moc_srcs} )

INSTALL_TARGETS( /lib OrcaProbeGuiQt )
