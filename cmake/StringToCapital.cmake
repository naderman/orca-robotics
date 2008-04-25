# ORCA_STRING_TO_CAPITAL( COMP_NAME )
#
# Sets component name and 3 other related variables:
# - APP_NAME        application executable name (defaults to the component name converted to lower case)
# - SERVICE_NAME    service library name (defaults to the component name prepended with 
#                   project name and appended with "Service")
#
MACRO( ORCA_STRING_TO_CAPITAL input_string OUTPUT_VAR )

    # jump through the hoops to construct project name string with the first character capitalized
    STRING( LENGTH ${input_string} string_length )

    MATH( EXPR string_length_minus_one "${string_length} - 1" )

    STRING( SUBSTRING ${input_string} 0 1 output_start )
    STRING( SUBSTRING ${input_string} 1 ${string_length_minus_one} output_rest )

    STRING( TOUPPER ${output_start} output_start_upper )
    STRING( TOLOWER ${output_rest} output_rest_lower )

    SET( ${OUTPUT_VAR} "${output_start_upper}${output_rest_lower}" )

ENDMACRO( ORCA_STRING_TO_CAPITAL input_string OUTPUT_VAR )
