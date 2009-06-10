include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# include_directories(
#     ${PROJECT_SOURCE_DIR}/src/components/tracermon/termutil
#     )

file( GLOB srcs *.cpp ../termutil/*.cpp )

set( lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LIBRARY( OrcaTracerMonTermNcurses DEFAULT ${lib_version} ${srcs} )
target_link_libraries( OrcaTracerMonTermNcurses ncurses form )
