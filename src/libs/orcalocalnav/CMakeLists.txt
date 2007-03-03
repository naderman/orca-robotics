IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

     INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       ${PROJECT_SOURCE_DIR}/src/components/localnav
       )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaLocalNav SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaLocalNavUtil )
    INSTALL_FILES( /include/orcalocalnav .*\\.h$ )
    
ENDIF ( NOT OS_WIN )