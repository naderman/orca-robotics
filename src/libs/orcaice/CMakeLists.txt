IF ( HYDROUTIL_INSTALLED )

    # these are the same as in UseComponentRules but without project rules
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )
    
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/detail )
    
    FILE( GLOB SRCS *.cpp detail/*.cpp )
    
    IF( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( OrcaIce SHARED ${SRCS} )
    ELSE ( NOT OS_WIN )
        # building shared libs is complicated in windows
        GLOBAL_ADD_LIBRARY( OrcaIce STATIC ${SRCS} )
    ENDIF ( NOT OS_WIN )
    
    TARGET_LINK_LIBRARIES( OrcaIce OrcaObjects )

    # special dependency on the generated files
    ADD_DEPENDENCIES( OrcaIce OrcaInterfaces )
    
    INSTALL_FILES( /include/orcaice .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ELSE ( HYDROUTIL_INSTALLED )
    GLOBAL_NOT_ADD_LIBRARY( OrcaIce "libHydroUtil not found" )
ENDIF ( HYDROUTIL_INSTALLED )