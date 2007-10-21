IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydro.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    
    GLOBAL_ADD_LIBRARY( OrcaBros1 SHARED convertutils.cpp )
    TARGET_LINK_LIBRARIES( OrcaBros1 HydroBros1 OrcaInterfaces )

    INSTALL_FILES( /include/orcabros1 .*\\.h$ )

ENDIF ( NOT OS_WIN )
