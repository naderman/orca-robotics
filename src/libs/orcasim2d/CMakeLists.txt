SET( lib_name OrcaSim2d )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

SET( ext_libs HydroSim2d )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} ${ext_libs} )

    IF( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE ( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF( NOT GBX_OS_WIN )

    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} ${ext_libs} )

    GBX_ADD_HEADERS( orcasim2d ${hdrs} )

#    IF( ORCA_BUILD_TESTS )
#        ADD_SUBDIRECTORY( test )
#    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
