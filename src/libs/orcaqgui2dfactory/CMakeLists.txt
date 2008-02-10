SET ( lib_name OrcaQGui2dFactory )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaQGui2d OrcaLogFactory ) # OrcaLogFactory needed for saving path to file (re-factor?)
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroNavUtil HydroQGui OrcaObjects )
REQUIRE_VAR( build LIB ${lib_name} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )
REQUIRE_VAR( build LIB ${lib_name} HYDROQGUI_INSTALLED "libHydroQGui not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_srcs *.cpp )
    
    SET( MOC_HEADERS
        pathfollower2delement.h
        pathinput.h
        pathplanner2delement.h
    )

    QT4_WRAP_CPP( MOC_srcs ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${HUMAN_srcs} ${MOC_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcaqgui2dfactory .*\\.h$ )
    INSTALL_FILES( /share/orcaqgui2dfactory ogHazardLookupTable.png )

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )
    
ENDIF ( build )
