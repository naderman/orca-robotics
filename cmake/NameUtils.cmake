# ORCA_SET_COMPONENT_NAME( COMPONENT_NAME )
#
# Sets component name and 3 other related variables:
# - COMP_NAME       component name.
# - APP_NAME        application executable name (defaults to the component name converted to lower case)
# - SERVICE_NAME    service library name (defaults to the component name prepended with 
#                   project name and appended with "Service")
# - COMP_NAMESPACE  component namespace (defaults to the component name converted to lower case)
#
# Also sets APP_CTRLC_HANDLER=1 (instructs application to install Ctrl-C Handler)
#
# For custom values, call this function first and then override some or all of the variables manually.
# For example:
#       ORCA_SET_COMPONENT_NAME( MyComponent )
#       set( COMP_NAMESPACE mycomp )
#
macro( ORCA_SET_COMPONENT_NAME c_name )
        
    set( COMP_NAME ${c_name} )

    # application executable name
    string( TOLOWER ${COMP_NAME} APP_NAME )

    # service lib name
    set( SERVICE_NAME ${PROJECT_NAME_CAP}${COMP_NAME}Service )

    # component namespace name
    set( COMP_NAMESPACE ${APP_NAME} )

    set( APP_CTRLC_HANDLER 1 )

endmacro( ORCA_SET_COMPONENT_NAME c_name )


# ORCA_SET_LIBRARY_NAME( LIBRARY_NAME )
#
# Sets library name and 2 other related variables:
# - LIB_NAME        library name.
# - LIB_NAMESPACE   library namespace (defaults to the library name converted to lower case).
# - LIB_INSTALL     the name of the subdirectory where header files are installed
#                   (defaults to the library name converted to lower case).
# For custom values, call this function first and then override some or all of the variables manually.
# For example:
#       ORCA_SET_LIBRARY_NAME( MyLibrary )
#       set( LIB_NAMESPACE mylib )
#    
macro( ORCA_SET_LIBRARY_NAME l_name )
        
    set( LIB_NAME ${l_name} )

    # library namespace name
    string( TOLOWER ${LIB_NAME} LIB_NAMESPACE )

    # component namespace name
    string( TOLOWER ${LIB_NAME} LIB_INSTALL )

endmacro( ORCA_SET_LIBRARY_NAME l_name )
