#
# Include and link against libOrcaIce and its dependencies
#
INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydroIceUtil.cmake )

INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )

LINK_LIBRARIES( OrcaIce )
