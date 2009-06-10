# ORCA_SET_COMPONENT_NAME( COMPONENT_NAME )
#
# Sets component name and 3 other related variables:
# - COMP_NAME       component name.
# - APP_NAME        application executable name (defaults to the component name converted to lower case)
# - SERVICE_NAME    service library name (defaults to the component name prepended with 
#                   project name and appended with "Service")
# - COMP_NAMESPACE  component namespace (defaults to the component name converted to lower case)
# - COMP_INTERFACE_FLAG
#                   Which standard interfaces the autognerated component will initialize.
#                   This variable must contain one of the values of orcaice::ComponentInterfaceFlag.
#                   Default value assigned by this CMake macro is orcaice::AllStandardInterfaces.
# - COMP_ADAPTER_POLICY
#                   How will the component's adapter be activated: automatically or manually.
#                   This variable must contain one of the values of orcaice::ComponentAdapterActivationPolicy.
#                   Default value assigned by this CMake macro is orcaice::AdapterAutoActivation.
#
# Also sets APP_CTRLC_HANDLER=1 (instructs application to install Ctrl-C Handler)
#
# For custom values, call this function first and then override some of the variables manually.
# For example:
#       ORCA_SET_COMPONENT_NAME( MyComponent )
#       set( COMP_NAMESPACE mycomp )
#
macro( ORCA_SET_COMPONENT_NAME c_name )
        
    set( COMP_NAME ${c_name} )

    # application executable name
    string( TOLOWER ${COMP_NAME} APP_NAME )

    # service lib name
    set( SERVICE_NAME ${GBX_PROJECT_NAME_CAP}${COMP_NAME}Service )

    # component namespace name
    set( COMP_NAMESPACE ${APP_NAME} )

    # which standard interfaces the component will provide.
    set( COMP_INTERFACE_FLAG "orcaice::AllStandardInterfaces" )

    # How will the component's adapter be activated
    set( COMP_ADAPTER_POLICY "orcaice::AdapterAutoActivation" )

    set( APP_CTRLC_HANDLER 1 )

endmacro( ORCA_SET_COMPONENT_NAME c_name )


# ORCA_SET_LIBRARY_NAME( LIBRARY_NAME )
#
# Sets library name and 2 other related variables:
# - LIB_NAME        library name.
# - LIB_NAMESPACE   library namespace (defaults to the library name converted to lower case).
# - LIB_INSTALL     the name of the subdirectory where header files are installed
#                   (defaults to the library name converted to lower case).
# For custom values, call this function first and then override some of the variables manually.
# For example:
#       ORCA_SET_LIBRARY_NAME( MyLibrary )
#       set( LIB_NAMESPACE mylib )
#    
macro( ORCA_SET_LIBRARY_NAME l_name )
        
    set( LIB_NAME ${l_name} )

    # library install subdirectory
    set( LIB_VERSION ${GBX_PROJECT_VERSION} )

    # library namespace name
    string( TOLOWER ${LIB_NAME} LIB_NAMESPACE )

    # library install subdirectory
    string( TOLOWER ${LIB_NAME} LIB_INSTALL )

endmacro( ORCA_SET_LIBRARY_NAME l_name )
