OPTION( BUILD_REGISTRY_VIEWER "Try to build RegistryView GUI" ON )

IF ( NOT OS_WIN )
IF ( BUILD_REGISTRY_VIEWER AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )

    SET( MOC_HEADERS
        mainwin.h
        regtreeview.h
        propertywidget.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

    GLOBAL_ADD_EXECUTABLE( registryview ${HUMAN_SRCS} ${MOC_SRCS} )
    TARGET_LINK_LIBRARIES( registryview OrcaQt OrcaQCompModel OrcaMisc )

    GENERATE_CONFIG_FILE( registryview.def )

ELSE ( BUILD_REGISTRY_VIEWER AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: RegistryView -- Qt4 not found")
    
ENDIF ( BUILD_REGISTRY_VIEWER AND QT4_FOUND )
ENDIF ( NOT OS_WIN )
