include_directories( 
    ${PROJECT_SOURCE_DIR}/src/hydrolibs
    ${PROJECT_SOURCE_DIR}/src           # for hydrointerfaces
    ${PROJECT_SOURCE_DIR}/src/hydrodrivers
    ${PROJECT_SOURCE_DIR}/src/libs
)

#
# Platform-specific compiler and linker flags
#
if( NOT ORCA_OS_WIN )
    add_definitions( "-Wall" )
else()
    add_definitions( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
endif()

if( NOT ORCA_MOTHERSHIP )
    include( ${ORCA_CMAKE_DIR}/UseOrca.cmake )

    #
    # Optional compile rules specific to a project
    #
    if( EXISTS ${PROJECT_SOURCE_DIR}/cmake/project/ProjectBasicRules.cmake )
        include( ${PROJECT_SOURCE_DIR}/cmake/project/ProjectBasicRules.cmake )
    endif()
endif()

