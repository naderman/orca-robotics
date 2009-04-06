#
# ORCA_GENERATE_CONFIG_FILE
#   Rule for generating .cfg files from .def files.
#
#       ORCA_GENERATE_CONFIG_FILE( def_file [component_target='DEF_FILE_WE'] [install_cfg=1] )
#
#   For .def file mycomponent.def, generates file mycomponent.cfg
#   If target name is not specified, it is assumed to be mycomponent.
#   Generated config files are installed by default, specify install_cfg=0 if installation
#   is not required (useful for config files used in tests).
#
macro( ORCA_GENERATE_CONFIG_FILE def_file )

    if( ${ARGC} LESS 1 )
        message( FATAL_ERROR "ORCA_GENERATE_CONFIG_FILE requires at least one input parameter." )
    endif( ${ARGC} LESS 1 )
    
    if( ${ARGC} GREATER 1 )
        # Target name is specified
        set( component_target ${ARGV1} )
    else( ${ARGC} GREATER 1 )
        # Target name is assumed from the def file name
        get_filename_component( component_target ${def_file} NAME_WE )
    #     message( STATUS "DEBUG: assumed component target name ${component_target}" )
    endif( ${ARGC} GREATER 1 )
    
    if( ${ARGC} GREATER 2 )
        # Target name is specified
        set( install_def ${ARGV2} )
        set( install_cfg ${ARGV2} )
        set( install_xml ${ARGV2} )
    else( ${ARGC} GREATER 2 )
        # Default behavior is to install the config file
        set( install_def 1 )
        set( install_cfg 1 )
        set( install_xml 1 )
    #     message( STATUS "DEBUG: assumed that the config file needs to be installed" )
    endif( ${ARGC} GREATER 2 )
    
    if( install_def )
        GBX_ADD_SHARED_FILES( def ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} )
#         INSTALL_FILES( /def FILES ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} )
    endif( install_def )
    
    # The standard CMAKE_CFG_INTDIR does not seem to get resolved in INSTALL_FILES, do it manually
    # alexm: I'm not sure if this is the right way to do it. I think this is a problem only 
    # when trying to install from command line. so maybe not all the variables are set.
    if( ORCA_OS_WIN )
        # VCC defaults to Debug
        set( manual_cfg_intdir "debug" )
    else( ORCA_OS_WIN )
        set( manual_cfg_intdir "." )
    endif( ORCA_OS_WIN )
    
    string( REGEX REPLACE "\\.def" ".cfg" cfg_file ${def_file} )
    add_custom_command( 
        TARGET ${component_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${cfg_file} from ${def_file}"
        COMMAND ${ORCA_DEF2CFG_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${cfg_file}
        # doesn't seem to work for some reason
    #     COMMENT "-- Generating ${cfg_file} from ${def_file}"
        )
    

    # add dependency on the generator executable when compiling Orca (satellite projects use installed version)
    # OR
    # when everything is built from one super-project.
    if( ORCA_MOTHERSHIP OR IS_SUPER_PROJECT )
        add_dependencies( ${component_target} def2cfg def2xml def2xmltemplate )
    endif( ORCA_MOTHERSHIP OR IS_SUPER_PROJECT )
    
    if( install_cfg )
        GBX_ADD_SHARED_FILES( cfg ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${cfg_file} )
#         INSTALL_FILES( /cfg FILES ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${cfg_file} )
    endif( install_cfg )
    
    # this is a global option
    if( ORCA_BUILD_XML )
        #
        # ICEGRID APPLICATION DESCRIPTOR XML FILE
        #
        string( REGEX REPLACE "\\.def" ".xml" xml_file ${def_file} )
        add_custom_command( 
            TARGET ${component_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "-- Generating ${xml_file} from ${def_file}"
            COMMAND ${ORCA_DEF2XML_COMMAND} --template ${CMAKE_CURRENT_SOURCE_DIR}/${def_file} ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${xml_file}
            # doesn't seem to work for some reason
#             COMMENT "-- Generating ${xml_file} from ${def_file}"
            )
        
        if( install_xml )
            GBX_ADD_SHARED_FILES( xml ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${xml_file} )
#             INSTALL_FILES( /xml FILES ${CMAKE_CURRENT_BINARY_DIR}/${manual_cfg_intdir}/${xml_file} )
        endif( install_xml )
    endif( ORCA_BUILD_XML )

endmacro( ORCA_GENERATE_CONFIG_FILE def_file )
