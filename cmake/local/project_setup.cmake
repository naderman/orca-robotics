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


#
# During the install we want to 'label' the install
#
INSTALL (SCRIPT ${PROJECT_SOURCE_DIR}/cmake/local/labelInstall.cmake)

