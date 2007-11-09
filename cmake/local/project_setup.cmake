MESSAGE( STATUS "== PROJECT-SPECIFIC SETUP ==" )
#
# Add project-specific setup
#

#
# Macro to get the location of deftools home
#
MACRO( FIND_DEFTOOLS DEFTOOLS_HOME )

    # set the path manually
    SET( ${DEFTOOLS_HOME} ${PROJECT_BINARY_DIR}/src/utils/deftools/${CMAKE_CFG_INTDIR} )

    # a note for the future: we can also ask cmake where this target going to be built
    # but this will only work if this target is already defined (right here it is not)
#     GET_TARGET_PROPERTY ( GENERATE_CFG_EXE_LOCATION generatecfg LOCATION )

ENDMACRO( FIND_DEFTOOLS DEFTOOLS_HOME )

# During the install we want to 'label' the install. The SYSTEM_HYDRO_INSTALL
# Variable is needed by the install script as this will most likely be run from
# Root and not have the compelete environment. For some reason I do not understand
# The 'SET - CACHED' variables are not available in the install scripts (djlm)
INSTALL (CODE "SET (SYSTEM_ORCA_SOURCE ${PROJECT_SOURCE_DIR})")
INSTALL (CODE "SET (SYSTEM_HYDRO_INSTALL ${HYDRO_HOME})")
INSTALL (SCRIPT ${PROJECT_SOURCE_DIR}/cmake/local/labelInstall.cmake)

