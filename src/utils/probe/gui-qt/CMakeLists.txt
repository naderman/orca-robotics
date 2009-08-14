include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

file( GLOB human_srcs *.cpp )

include_directories(
    ${PROJECT_SOURCE_DIR}/src/libs
)

set( moc_hdrs
    guiqtdisplay.h
    mainwin.h
    probeview.h
    probedelegate.h
)

QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

set( lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LIBRARY( OrcaProbeGuiQt DEFAULT ${lib_version} ${human_srcs} ${moc_srcs} )
