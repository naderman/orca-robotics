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
MACRO( ORCA_SET_COMPONENT_NAME C_NAME )
        
    SET( COMP_NAME ${C_NAME} )

    # application executable name
    STRING( TOLOWER ${COMP_NAME} APP_NAME )

    # service lib name
    # jump through the hoops to construct project name string with the first character capitalized
    STRING( LENGTH ${PROJECT_NAME} PROJECT_NAME_LENGTH )
    MATH( EXPR PROJECT_NAME_LENGTH_MINUS_ONE "${PROJECT_NAME_LENGTH} - 1" )
    STRING( SUBSTRING ${PROJECT_NAME} 0 1 PROJECT_NAME_START )
    STRING( SUBSTRING ${PROJECT_NAME} 1 ${PROJECT_NAME_LENGTH_MINUS_ONE} PROJECT_NAME_REST )
    STRING( TOUPPER ${PROJECT_NAME_START} PROJECT_NAME_START_UPPER )

    SET( SERVICE_NAME ${PROJECT_NAME_START_UPPER}${PROJECT_NAME_REST}${COMP_NAME}Service )

    # component namespace name
    SET( COMP_NAMESPACE ${APP_NAME} )

    SET( APP_CTRLC_HANDLER 1 )

ENDMACRO( ORCA_SET_COMPONENT_NAME C_NAME )
