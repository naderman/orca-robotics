#
# Rules common to all "standard" components
#
include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#
# Common macro definitions
#
include( ${ORCA_CMAKE_DIR}/WrapComponent.cmake )
include( ${ORCA_CMAKE_DIR}/GenerateConfigFile.cmake )

if( NOT ORCA_MOTHERSHIP )
    # includes UseOrca.cmake
    include( ${ORCA_CMAKE_DIR}/UseOrcaInterfaces.cmake )

    #
    # Optional component compile rules specific to a project
    #
    if( EXISTS ${PROJECT_SOURCE_DIR}/cmake/project/ProjectComponentRules.cmake )
        include( ${PROJECT_SOURCE_DIR}/cmake/project/ProjectComponentRules.cmake )
    endif()
endif()
