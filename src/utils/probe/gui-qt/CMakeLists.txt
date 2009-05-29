include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

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

GBX_ADD_LIBRARY( OrcaProbeGuiQt DEFAULT ${human_srcs} ${moc_srcs} )
