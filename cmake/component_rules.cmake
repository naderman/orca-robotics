#
# Rules common to all componetns
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${HYDRO_HOME}/cmake/UseHydro.cmake )

#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_rules.cmake )
