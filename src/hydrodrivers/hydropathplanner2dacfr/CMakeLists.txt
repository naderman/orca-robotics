ORCA_SET_LIBRARY_NAME( HydroPathPlanner2dAcfr )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "Only Linux OS is supported" )

set( dep_libs HydroInterfaces HydroPathPlan )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    if( QT4_FOUND )
        include( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        add_definitions( -DQT4_FOUND )
    else( QT4_FOUND )
        list( REMOVE_ITEM srcs "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )
        message( STATUS "     Qt4 not found: skeletongraphics will not be built" )
    endif( QT4_FOUND )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST_WITH_CONFIG( PathPlanner2d pathplanner2dacfr driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif(build )
