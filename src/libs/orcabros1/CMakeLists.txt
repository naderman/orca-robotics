IF ( NOT OS_WIN )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydro.cmake )

    GLOBAL_ADD_LIBRARY( OrcaBros1 SHARED convertutils.cpp )
    TARGET_LINK_LIBRARIES( OrcaBros1 HydroBros1 OrcaInterfaces )

    INSTALL_FILES( /include/orcabros1 .*\\.h$ )

ENDIF ( NOT OS_WIN )
