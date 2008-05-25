ORCA_SET_COMPONENT_NAME( Slice2String )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs IceUtil Slice )

    # we are using internal Ice API which is different between Ice versions

    # Check Ice version
    EXEC_PROGRAM ( ${ICE_HOME}/bin/slice2cpp ARGS --version OUTPUT_VARIABLE ICE_VERSION )
    #MESSAGE( STATUS "DEBUG: Ice version: '${ICE_VERSION}'")
    
    # notice that this allows beta versions, e.g. 3.3b
    IF( ICE_VERSION MATCHES "3\\.[3-9]" )
        MESSAGE( STATUS "DEBUG: this is Ice 3.3")
        ADD_DEFINITIONS( "-DICE_VERSION_33" )
    ENDIF( ICE_VERSION MATCHES "3\\.[3-9]" )
    
    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
