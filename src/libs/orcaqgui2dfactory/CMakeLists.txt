SET ( lib_name OrcaQGui2dFactory )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaLogFactory ) # OrcaLogFactory needed for saving path to file (re-factor?)
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroNavUtil HydroQGui HydroQGuiPaint OrcaObjects OrcaQGuiElementUtil )
REQUIRE_VAR( build LIB ${lib_name} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )
REQUIRE_VAR( build LIB ${lib_name} HYDROQGUI_INSTALLED "libHydroQGui not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB human_srcs *.cpp )
    
    SET( moc_hdrs
        pathfollower2delement.h
        pathinput.h
        pathplanner2delement.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} ${moc_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcaqgui2dfactory .*\\.h$ )
    INSTALL_FILES( /share/orcaqgui2dfactory ogHazardLookupTable.png )

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )
    
ENDIF ( build )
