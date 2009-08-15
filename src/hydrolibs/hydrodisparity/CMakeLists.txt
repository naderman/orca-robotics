ORCA_SET_LIBRARY_NAME( HydroDisparity )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not supported" )

find_package( OpenGL )

include( ${ORCA_CMAKE_DIR}/FindGLEW.cmake )

GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GLEW_FOUND "GLEW not found" )

set( dep_libs HydroGlu )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
    
    set( QT_USE_QTOPENGL TRUE )
    include( ${QT_USE_FILE} )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    
    target_link_libraries( ${LIB_NAME} ${dep_libs} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )


endif(build )

