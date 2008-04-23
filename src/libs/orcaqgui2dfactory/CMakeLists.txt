SET( lib_name OrcaQGui2dFactory )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( ext_libs ${dep_libs} HydroNavUtil HydroQGui HydroQGuiElementUtil HydroOgMap )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

SET( int_libs OrcaLogFactory OrcaObjects OrcaQGuiElementUtil OrcaOgMap ) # OrcaLogFactory needed for saving path to file (re-factor?)
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} )
    
    SET( moc_hdrs
        pathfollower2delement.h
        pathinput.h
        pathplanner2delement.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} ${moc_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )

    GBX_ADD_HEADERS( orcaqgui2dfactory ${hdrs} )
    GBX_ADD_SHARED_FILES( orcaqgui2dfactory ogHazardLookupTable.png )

    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )
    
ENDIF( build )
