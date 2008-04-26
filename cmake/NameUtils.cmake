# ORCA_SET_COMPONENT_NAME( COMP_NAME )
#
# Sets component name and 3 other related variables:
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
#       SET( COMP_NAMESPACE mycomp )
#
MACRO( ORCA_SET_COMPONENT_NAME c_name )
        
    SET( COMP_NAME ${c_name} )

    # application executable name
    STRING( TOLOWER ${COMP_NAME} APP_NAME )

    # service lib name
    SET( SERVICE_NAME ${PROJECT_NAME_CAP}${COMP_NAME}Service )

    # component namespace name
    SET( COMP_NAMESPACE ${APP_NAME} )

    SET( APP_CTRLC_HANDLER 1 )

ENDMACRO( ORCA_SET_COMPONENT_NAME c_name )


# ORCA_SET_LIBRARY_NAME( COMP_NAME )
#
# Sets library name and other related variables:
# - LIB_NAMESPACE   library namespace (defaults to the library name converted to lower case).
# - LIB_INSTALL     the name of the subdirectory where header files are installed
#                   (defaults to the library name converted to lower case).
# For custom values, call this function first and then override some or all of the variables manually.
# For example:
#       ORCA_SET_LIBRARY_NAME( MyLibrary )
#       SET( LIB_NAMESPACE mylib )
#    
MACRO( ORCA_SET_LIBRARY_NAME l_name )
        
    SET( LIB_NAME ${l_name} )

    # library namespace name
    STRING( TOLOWER ${LIB_NAME} LIB_NAMESPACE )

    # component namespace name
    STRING( TOLOWER ${LIB_NAME} LIB_INSTALL )

ENDMACRO( ORCA_SET_LIBRARY_NAME l_name )