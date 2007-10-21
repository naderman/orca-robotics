#
# Include and link against libOrcaIce and its dependencies
#
INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/libs
)

INCLUDE( ${HYDRO_HOME}/cmake/UseHydro.cmake )
INCLUDE( ${HYDRO_HOME}/cmake/UseHydroUtil.cmake )

LINK_LIBRARIES( OrcaIce OrcaObjects )
