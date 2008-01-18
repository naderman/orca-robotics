SET ( LIB_NAME OrcaQGuiElementUtil )
GLOBAL_ADD_LICENSE( GPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${HUMAN_SRCS} )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ) # OrcaQt OrcaQCompModel HydroQGui )    
    INSTALL_FILES( /include/orcaqguielementutil .*\\.h$ )

ENDIF ( BUILD )
