#
# Rules common to all "standard" componetns
#
INCLUDE( ${HYDRO_CMAKE_DIR}/UseGearbox.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
# we already tested that enough of Ice is installed
INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
# we already tested that enough of Hydro is installed
INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroIceUtil.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/WrapComponent.cmake )

LINK_LIBRARIES( GbxUtilAcfr GbxIceUtilAcfr HydroUtil HydroIceUtil OrcaIce )

#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_rules.cmake )
