set( exe_name listcameras )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${exe_name} ON )

include( ${ORCA_CMAKE_DIR}/FindDc1394.cmake )
GBX_REQUIRE_VAR( build EXE ${exe_name} DC1394_FOUND "libdc1394v2 (dc1394 ver 2) was not found" )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
#    INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)

    # Source files
    file( GLOB srcs *.cpp *.c )
    set( dep_libs dc1394 raw1394)
   
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99" )
    GBX_ADD_EXECUTABLE( ${exe_name} ${srcs} )
    target_link_libraries( ${exe_name} ${dep_libs} )

endif( build )
