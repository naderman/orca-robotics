IF ( OS_LINUX )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaPathPlan SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaPathPlan HydroPathPlan )
    INSTALL_FILES( /include/orcapathplan .*\\.h$ )
    
ENDIF ( OS_LINUX )
