#
# Include and link against libOrcaIce and its dependencies
#
include( ${HYDRO_CMAKE_DIR}/UseHydroIceUtil.cmake )

include( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )

link_libraries( OrcaIce )
