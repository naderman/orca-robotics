SET ( LIB_NAME OrcaQGui2dFactory )
GLOBAL_ADD_LICENSE( GPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

SET( DEP_LIBS OrcaQGui2d OrcaLogFactory ) # OrcaLogFactory needed for saving path to file (re-factor?)
REQUIRE_TARGETS( BUILD LIB ${LIB_NAME} ${DEP_LIBS} )

SET( DEP_LIBS ${DEP_LIBS} HydroNavUtil HydroQGui )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} HYDROQGUI_INSTALLED "libHydroQGui not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    SET( MOC_HEADERS
        pathfollower2delement.h
        pathinput.h
        pathplanner2delement.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${DEP_LIBS} )

    INSTALL_FILES( /include/orcaqgui2dfactory .*\\.h$ )
    INSTALL_FILES( /share/orcaqgui2dfactory ogHazardLookupTable.png )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( BUILD )
