#
# Rules common to all "standard" componetns
#
include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#
# Common macro definitions
#
include( ${ORCA_CMAKE_DIR}/WrapComponent.cmake )
include( ${ORCA_CMAKE_DIR}/GenerateConfigFile.cmake )

#
# Project-specific rules
#
include( ${PROJECT_SOURCE_DIR}/cmake/internal/project_rules.cmake )
