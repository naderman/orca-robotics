#
# GENERATE_CONFIG_FILE
#   Rule for generating .cfg files from .def files.
#
#       GENERATE_CONFIG_FILE( DEF_FILE [COMPONENT_TARGET='DEF_FILE_WE'] [INSTALL_CFG=1] )
#
#   For .def file mycomponent.def, generates file mycomponent.cfg
#   If target name is not specified, it is assumed to be mycomponent.
#   Generated config files are installed by default, specify INSTALL_CFG=0 if installation
#   is not required (useful for config files used in tests).
#
MACRO( GENERATE_CONFIG_FILE DEF_FILE )

    IF ( ${ARGC} LESS 1 )
        MESSAGE( FATAL_ERROR "GENERATE_CONFIG_FILE requires at least one input parameter." )
    ENDIF ( ${ARGC} LESS 1 )
    
    IF ( ${ARGC} GREATER 1 )
        # Target name is specified
        SET ( COMPONENT_TARGET ${ARGV1} )
    ELSE ( ${ARGC} GREATER 1 )
        # Target name is assumed from the def file name
        GET_FILENAME_COMPONENT( COMPONENT_TARGET ${DEF_FILE} NAME_WE )
    #     MESSAGE ( STATUS "DEBUG: assumed component target name ${COMPONENT_TARGET}" )
    ENDIF ( ${ARGC} GREATER 1 )
    
    IF ( ${ARGC} GREATER 2 )
        # Target name is specified
        SET ( INSTALL_DEF ${ARGV2} )
        SET ( INSTALL_CFG ${ARGV2} )
        SET ( INSTALL_XML ${ARGV2} )
    ELSE ( ${ARGC} GREATER 2 )
        # Default behavior is to install the config file
        SET ( INSTALL_DEF 1 )
        SET ( INSTALL_CFG 1 )
        SET ( INSTALL_XML 1 )
    #     MESSAGE ( STATUS "DEBUG: assumed that the config file needs to be installed" )
    ENDIF ( ${ARGC} GREATER 2 )
    
    IF ( INSTALL_DEF )
        INSTALL_FILES( /def FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} )
    ENDIF ( INSTALL_DEF )
    
    # The standard CMAKE_CFG_INTDIR does not seem to get resolved in INSTALL_FILES, do it manually
    # alexm: I'm not sure if this is the right way to do it. I think this is a problem only 
    # when trying to install from command line. so maybe not all the variables are set.
    IF( OS_WIN )
        # VCC defaults to Debug
        SET( MANUAL_CFG_INTDIR "debug" )
    ELSE( OS_WIN )
        SET( MANUAL_CFG_INTDIR "." )
    ENDIF( OS_WIN )
    
    STRING( REGEX REPLACE "\\.def" ".cfg" CFG_FILE ${DEF_FILE} )
    ADD_CUSTOM_COMMAND( 
        TARGET ${COMPONENT_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${CFG_FILE} from ${DEF_FILE}"
        COMMAND ${ORCA_DEF2CFG_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${CFG_FILE}
        # doesn't seem to work for some reason
    #     COMMENT "-- Generating ${CFG_FILE} from ${DEF_FILE}"
        )
    
    IF ( ORCA_MOTHERSHIP )
        ADD_DEPENDENCIES( ${COMPONENT_TARGET} def2cfg def2xml def2xmltemplate )
    ENDIF ( ORCA_MOTHERSHIP )
    
    IF ( INSTALL_CFG )
        INSTALL_FILES( /cfg FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${CFG_FILE} )
    ENDIF ( INSTALL_CFG )
    
    # this is a global option
    IF ( GENERATE_XML )
        #
        # ICEGRID APPLICATION DESCRIPTOR XML FILE
        #
        STRING( REGEX REPLACE "\\.def" ".xml" XML_FILE ${DEF_FILE} )
        ADD_CUSTOM_COMMAND( 
            TARGET ${COMPONENT_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${XML_FILE} from ${DEF_FILE}"
            COMMAND ${ORCA_DEF2XML_COMMAND} --template ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${XML_FILE}
            # doesn't seem to work for some reason
#             COMMENT "-- Generating ${XML_FILE} from ${DEF_FILE}"
            )
        
        IF ( INSTALL_XML )
            INSTALL_FILES( /xml FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${XML_FILE} )
        ENDIF ( INSTALL_XML )
        
        # as of version 2.7.0, the template is included in the regural xml file
        #
        # ICEGRID SERVER TEMPLATE XML FILE
        #
#         STRING( REGEX REPLACE "\\.def" "-template.xml" TEMPLATE_XML_FILE ${DEF_FILE} )
#         ADD_CUSTOM_COMMAND( 
#             TARGET ${COMPONENT_TARGET} POST_BUILD
#             COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${TEMPLATE_XML_FILE} from ${DEF_FILE}"
#             COMMAND ${ORCA_DEF2XMLTEMPLATE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${DEF_FILE} 
#                 ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${TEMPLATE_XML_FILE}
#             # doesn't seem to work for some reason
#         #     COMMENT "-- Generating ${XML_FILE} from ${DEF_FILE}"
#             )
#         
#         IF ( INSTALL_XML )
#             INSTALL_FILES( /xml FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANUAL_CFG_INTDIR}/${TEMPLATE_XML_FILE} )
#         ENDIF ( INSTALL_XML )
    ENDIF ( GENERATE_XML )

ENDMACRO( GENERATE_CONFIG_FILE DEF_FILE )
