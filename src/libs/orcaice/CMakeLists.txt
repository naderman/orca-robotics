SET ( LIB_NAME OrcaIce )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} HYDROUTIL_INSTALLED "libHydroUtil not found" )

# SET( DEP_LIBS OrcaObjects )
# REQUIRE_TARGETS( BUILD LIB ${LIB_NAME} ${DEP_LIBS} )

IF ( BUILD )

    # these are the same as in UseComponentRules but without project rules
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroIceUtil.cmake )
    
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/detail )
    
    FILE( GLOB SRCS *.cpp detail/*.cpp )
    
    IF( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    ELSE ( NOT OS_WIN )
        # building shared libs is complicated in windows
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} STATIC ${SRCS} )
    ENDIF ( NOT OS_WIN )
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEP_LIBS} )

    # special dependency on the generated files
    ADD_DEPENDENCIES( ${LIB_NAME} OrcaInterfaces )
    
    INSTALL_FILES( /include/orcaice .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD )
