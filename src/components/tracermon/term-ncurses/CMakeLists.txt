INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# INCLUDE_DIRECTORIES(
#     ${PROJECT_SOURCE_DIR}/src/components/tracermon/termutil
#     )

FILE( GLOB srcs *.cpp ../termutil/*.cpp )

GBX_ADD_LIBRARY( OrcaTracerMonTermNcurses SHARED ${srcs} )
TARGET_LINK_LIBRARIES( OrcaTracerMonTermNcurses ncurses form )
