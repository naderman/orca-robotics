#
# Rules common to all "standard" componetns
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

INCLUDE( ${ORCA_CMAKE_DIR}/WrapComponent.cmake )

#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/internal/project_rules.cmake )
