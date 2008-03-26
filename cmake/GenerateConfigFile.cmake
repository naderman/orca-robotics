#
# GENERATE_CONFIG_FILE
#   Rule for generating .cfg files from .def files.
#
#       GENERATE_CONFIG_FILE( def_file [component_target='DEF_FILE_WE'] [install_cfg=1] )
#
#   For .def file mycomponent.def, generates file mycomponent.cfg
#   If target name is not specified, it is assumed to be mycomponent.
#   Generated config files are installed by default, specify install_cfg=0 if installation
#   is not required (useful for config files used in tests).
#
MACRO( GENERATE_CONFIG_FILE def_file )

    IF ( ${ARGC} LESS 1 )
        MESSAGE( FATAL_ERROR "GENERATE_CONFIG_FILE requires at least one input parameter." )
    ENDIF ( ${ARGC} LESS 1 )
    
    IF ( ${ARGC} GREATER 1 )
        # Target name is specified
        SET ( component_target ${ARGV1} )
    ELSE ( ${ARGC} GREATER 1 )
        # Target name is assumed from the def file name
        GET_FILENAME_COMPONENT( component_target ${def_file} NAME_WE )
    #     MESSAGE ( STATUS "DEBUG: assumed component target name ${component_target}" )
    ENDIF ( ${ARGC} GREATER 1 )
    
    IF ( ${ARGC} GREATER 2 )
        # Target name is specified
        SET ( install_def ${ARGV2} )
        SET ( install_cfg ${ARGV2} )
        SET ( install_xml ${ARGV2} )
    ELSE ( ${ARGC} GREATER 2 )
        # Default behavior is to install the config file
        SET ( install_def 1 )
        SET ( install_cfg 1 )
        SET ( install_xml 1 )
    #     MESSAGE ( STATUS "DEBUG: assumed that the config file needs to be installed" )
    ENDIF ( ${ARGC} GREATER 2 )
    
    IF ( install_def )
        GBX_ADD_SHARED_FILES( def ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} )
#         INSTALL_FILES( /def FILES ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} )
    ENDIF ( install_def )
    
    # The standard CMAKE_CFG_INTDIR does not seem to get resolved in INSTALL_FILES, do it manually
    # alexm: I'm not sure if this is the right way to do it. I think this is a problem only 
    # when trying to install from command line. so maybe not all the variables are set.
    IF( ORCA_OS_WIN )
        # VCC defaults to Debug
        SET( manual_cfg_intdir "debug" )
    ELSE( ORCA_OS_WIN )
        SET( manual_cfg_intdir "." )
    ENDIF( ORCA_OS_WIN )
    
    STRING( REGEX REPLACE "\\.def" ".cfg" cfg_file ${def_file} )
    ADD_CUSTOM_COMMAND( 
        TARGET ${component_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${cfg_file} from ${def_file}"
        COMMAND ${ORCA_DEF2CFG_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${cfg_file}
        # doesn't seem to work for some reason
    #     COMMENT "-- Generating ${cfg_file} from ${def_file}"
        )
    
    IF ( ORCA_MOTHERSHIP )
        ADD_DEPENDENCIES( ${component_target} def2cfg def2xml def2xmltemplate )
    ENDIF ( ORCA_MOTHERSHIP )
    
    IF ( install_cfg )
        GBX_ADD_SHARED_FILES( cfg ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${cfg_file} )
#         INSTALL_FILES( /cfg FILES ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${cfg_file} )
    ENDIF ( install_cfg )
    
    # this is a global option
    IF ( ORCA_BUILD_XML )
        #
        # ICEGRID APPLICATION DESCRIPTOR XML FILE
        #
        STRING( REGEX REPLACE "\\.def" ".xml" xml_file ${def_file} )
        ADD_CUSTOM_COMMAND( 
            TARGET ${component_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${xml_file} from ${def_file}"
            COMMAND ${ORCA_DEF2XML_COMMAND} --template ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${xml_file}
            # doesn't seem to work for some reason
#             COMMENT "-- Generating ${xml_file} from ${def_file}"
            )
        
        IF ( install_xml )
            GBX_ADD_SHARED_FILES( xml ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${xml_file} )
#             INSTALL_FILES( /xml FILES ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${xml_file} )
        ENDIF ( install_xml )
    ENDIF ( ORCA_BUILD_XML )

ENDMACRO( GENERATE_CONFIG_FILE def_file )
