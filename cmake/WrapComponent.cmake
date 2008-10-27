#
# ORCA_WRAP_COMPONENT_INTO_APP ( APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
MACRO( ORCA_WRAP_COMPONENT_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE )

#     SET( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/automain.cpp )
    SET( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/main.cpp.template ${MAIN_CPP_FILE} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

ENDMACRO( ORCA_WRAP_COMPONENT_INTO_APP APP_NAME APP_CTRLC_HANDLER  COMP_NAMESPACE )

#
# ORCA_WRAP_COMPONENT_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
MACRO( ORCA_WRAP_COMPONENT_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE )

#     SET( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autoservice.h )
#     SET( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autoservice.cpp )
    SET( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    SET( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.h.template ${SERVICE_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.cpp.template ${SERVICE_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].template files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

ENDMACRO( ORCA_WRAP_COMPONENT_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE )

#
# ORCA_WRAP_MAINTHREAD_INTO_APP ( APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
MACRO( ORCA_WRAP_MAINTHREAD_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME )

    SET( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )
    SET( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    SET( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/main.cpp.template ${MAIN_CPP_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_thread.h.template ${COMPONENT_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_thread.cpp.template ${COMPONENT_CPP_FILE} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} ${COMPONENT_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

ENDMACRO( ORCA_WRAP_MAINTHREAD_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME )

#
# ORCA_WRAP_MAINTHREAD_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
MACRO( ORCA_WRAP_MAINTHREAD_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )

    SET( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    SET( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )
    SET( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    SET( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.h.template ${SERVICE_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.cpp.template ${SERVICE_CPP_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_thread.h.template ${COMPONENT_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_thread.cpp.template ${COMPONENT_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].template files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} ${COMPONENT_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

ENDMACRO( ORCA_WRAP_MAINTHREAD_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )


#
# ORCA_WRAP_MAINSUBSYSTEM_INTO_APP ( APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainsubsystem.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainSubsystem
# - which derives from hydroiceutil::Thread or its derivatives
#
MACRO( ORCA_WRAP_MAINSUBSYSTEM_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME )

    SET( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )
    SET( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    SET( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/main.cpp.template ${MAIN_CPP_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_subsys.h.template ${COMPONENT_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_subsys.cpp.template ${COMPONENT_CPP_FILE} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} ${COMPONENT_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

ENDMACRO( ORCA_WRAP_MAINSUBSYSTEM_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME )

#
# ORCA_WRAP_MAINSUBSYSTEM_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainsubsystem.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainSubsystem
# - which derives from hydroiceutil::Thread or its derivatives
#
MACRO( ORCA_WRAP_MAINSUBSYSTEM_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )

    SET( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    SET( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )
    SET( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    SET( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.h.template ${SERVICE_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/service.cpp.template ${SERVICE_CPP_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_subsys.h.template ${COMPONENT_H_FILE} )
    CONFIGURE_FILE( ${ORCA_CMAKE_DIR}/component_with_subsys.cpp.template ${COMPONENT_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].template files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} ${COMPONENT_CPP_FILE} )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

ENDMACRO( ORCA_WRAP_MAINSUBSYSTEM_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )
