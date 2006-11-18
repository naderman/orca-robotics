MESSAGE( STATUS "== PROJECT-SPECIFIC SETUP ==" )
#
# Add project-specific setup
#

#
# Macro to get the location of generatecfg utility
#
MACRO( FIND_GENERATE_CFG GENERATE_CFG_HOME )

    # set the path manually
    SET( ${GENERATE_CFG_HOME} ${PROJECT_BINARY_DIR}/src/utils/orcadef/${CMAKE_CFG_INTDIR} )

    # a note for the future: we can also ask cmake where this target going to be built
    # but this will only work if this target is already defined (right here it is not)
#     GET_TARGET_PROPERTY ( GENERATE_CFG_EXE_LOCATION generatecfg LOCATION )

ENDMACRO( FIND_GENERATE_CFG GENERATE_CFG_HOME )
