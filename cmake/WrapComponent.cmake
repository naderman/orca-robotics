#
# WRAP_COMPONENT_INTO_APP ( APP_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
MACRO ( WRAP_COMPONENT_INTO_APP APP_NAME COMP_NAMESPACE )

#     SET ( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/automain.cpp )
    SET ( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/main.cpp.template ${MAIN_CPP_FILE} )

    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

ENDMACRO ( WRAP_COMPONENT_INTO_APP APP_NAME COMP_NAMESPACE )

#
# WRAP_COMPONENT_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
MACRO ( WRAP_COMPONENT_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE )

#     SET ( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autoservice.h )
#     SET ( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autoservice.cpp )
    SET ( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    SET ( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.h.template ${SERVICE_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.cpp.template ${SERVICE_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].template files.
    GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

ENDMACRO ( WRAP_COMPONENT_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE )

#
# WRAP_MAINTHREAD_INTO_APP ( APP_NAME COMP_NAMESPACE COMP_NAME SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
MACRO ( WRAP_MAINTHREAD_INTO_APP APP_NAME COMP_NAMESPACE COMP_NAME )

    SET ( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )
    SET ( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    SET ( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/main.cpp.template ${MAIN_CPP_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component.h.template ${COMPONENT_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component.cpp.template ${COMPONENT_CPP_FILE} )

    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} ${COMPONENT_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

ENDMACRO ( WRAP_MAINTHREAD_INTO_APP APP_NAME COMP_NAMESPACE COMP_NAME )

#
# WRAP_MAINTHREAD_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
MACRO ( WRAP_MAINTHREAD_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )

    SET ( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    SET ( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )
    SET ( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    SET ( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.h.template ${SERVICE_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.cpp.template ${SERVICE_CPP_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component.h.template ${COMPONENT_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component.cpp.template ${COMPONENT_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].template files.
    GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} ${COMPONENT_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

ENDMACRO ( WRAP_MAINTHREAD_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )

#
# SET_COMPONENT_NAMES ( COMP_NAME )
#
# Sets component name and 3 other related variables:
# - APP_NAME        application executable name (defaults to the component name converted to lower case)
# - SERVICE_NAME    service library name (defaults to the component name prepended with 
#                   project name and appended with "Service")
# - COMP_NAMESPACE  component namespace (defaults to the component name converted to lower case)
#
# For custom values, call this function first and then override some or all of the variables manually.
# For example:
#       SET_COMPONENT_NAMES ( MyComponent )
#       SET ( COMP_NAMESPACE mycomp )
#
MACRO ( SET_COMPONENT_NAMES C_NAME )
        
    SET ( COMP_NAME ${C_NAME} )

    # application executable name
    STRING ( TOLOWER ${COMP_NAME} APP_NAME )

    # service lib name
    # jump through the hoops to construct project name string with the first character capitalized
    STRING ( LENGTH ${PROJECT_NAME} PROJECT_NAME_LENGTH )
    MATH ( EXPR PROJECT_NAME_LENGTH_MINUS_ONE "${PROJECT_NAME_LENGTH} - 1" )
    STRING ( SUBSTRING ${PROJECT_NAME} 0 1 PROJECT_NAME_START )
    STRING ( SUBSTRING ${PROJECT_NAME} 1 ${PROJECT_NAME_LENGTH_MINUS_ONE} PROJECT_NAME_REST )
    STRING ( TOUPPER ${PROJECT_NAME_START} PROJECT_NAME_START_UPPER )

    SET ( SERVICE_NAME ${PROJECT_NAME_START_UPPER}${PROJECT_NAME_REST}${COMP_NAME}Service )

    # component namespace name
    SET ( COMP_NAMESPACE ${APP_NAME} )

ENDMACRO ( SET_COMPONENT_NAMES C_NAME )
