#
# Rules common to all "standard" componetns
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
# we already tested that enough of Ice is installed
INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
# we already tested that enough of Hydro is installed
INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroIceUtil.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/WrapComponent.cmake )

# (other basic libs are linked from the UseHydroIceUtil script)
LINK_LIBRARIES( OrcaIce )

#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_rules.cmake )
