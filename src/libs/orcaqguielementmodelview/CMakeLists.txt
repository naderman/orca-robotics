SET ( lib_name OrcaQGuiElementModelView )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaQt OrcaQCompModel )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroQGui )
REQUIRE_VAR( build LIB ${lib_name} HYDROQGUI_INSTALLED "libHydroQGui not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_srcs *.cpp )
    
    SET( MOC_HEADERS
        guielementmodel.h
        guielementview.h
#        mainwin.h
        regselectview.h
#        shortcutaction.h
    )

    QT4_WRAP_CPP( MOC_srcs ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${HUMAN_srcs} ${MOC_srcs} )
    
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )
    
    INSTALL_FILES( /include/orcaqguielementmodelview .*\\.h$ )

ENDIF ( build )
