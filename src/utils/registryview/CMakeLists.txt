OPTION( BUILD_REGISTRY_VIEWER "Try to build RegistryView GUI" ON )

IF ( NOT WIN32 )
IF ( BUILD_REGISTRY_VIEWER AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )

    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    LINK_DIRECTORIES(
        ${PROJECT_BINARY_DIR}/src/utils/qorcacm
    )

    #
    # A list of files to MOC
    #
    SET( MOC_HEADERS
        mainwin.h
        regtreeview.h
        propertywidget.h
    )
    
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

    GLOBAL_ADD_EXECUTABLE( registryview ${HUMAN_SRCS} ${MOC_SRCS} )
    TARGET_LINK_LIBRARIES( registryview OrcaQCompModel OrcaMisc ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    GENERATE_CONFIG_FILE( registryview.def )

ELSE ( BUILD_REGISTRY_VIEWER AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: RegistryView -- Qt4 not found")
    
ENDIF ( BUILD_REGISTRY_VIEWER AND QT4_FOUND )
ENDIF ( NOT WIN32 )
