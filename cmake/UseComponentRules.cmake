#
# Rules common to all "standard" componetns
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#
# Common macro definitions
#
INCLUDE( ${ORCA_CMAKE_DIR}/WrapComponent.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/GenerateConfigFile.cmake )
# Obsolete but still used by Probe and a few other places
INCLUDE( ${ORCA_CMAKE_DIR}/OptionalSubLibrary.cmake )

#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/internal/project_rules.cmake )
