#
# ORCA_WRAP_COMPONENT_INTO_APP ( APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
macro( ORCA_WRAP_COMPONENT_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE )

#     set( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/automain.cpp )
    set( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/main.cpp.in ${MAIN_CPP_FILE} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

endmacro( ORCA_WRAP_COMPONENT_INTO_APP APP_NAME APP_CTRLC_HANDLER  COMP_NAMESPACE )

#
# ORCA_WRAP_COMPONENT_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
macro( ORCA_WRAP_COMPONENT_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE )

#     set( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autoservice.h )
#     set( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autoservice.cpp )
    set( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    set( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/service.h.in ${SERVICE_H_FILE} )
    configure_file( ${ORCA_CMAKE_DIR}/service.cpp.in ${SERVICE_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].in files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

endmacro( ORCA_WRAP_COMPONENT_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE )

#
# ORCA_WRAP_MAINTHREAD_INTO_APP ( APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
macro( ORCA_WRAP_MAINTHREAD_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME )

    set( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )
    set( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    set( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/main.cpp.in ${MAIN_CPP_FILE} )
    configure_file( ${ORCA_CMAKE_DIR}/component.h.in ${COMPONENT_H_FILE} )
    configure_file( ${ORCA_CMAKE_DIR}/component.cpp.in ${COMPONENT_CPP_FILE} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${MAIN_CPP_FILE} ${COMPONENT_CPP_FILE} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

endmacro( ORCA_WRAP_MAINTHREAD_INTO_APP APP_NAME APP_CTRLC_HANDLER COMP_NAMESPACE COMP_NAME )

#
# ORCA_WRAP_MAINTHREAD_INTO_SERVICE ( SERVICE_NAME COMP_NAMESPACE SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# Expects to find 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
macro( ORCA_WRAP_MAINTHREAD_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )

    set( SERVICE_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    set( SERVICE_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )
    set( COMPONENT_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    set( COMPONENT_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/service.h.in ${SERVICE_H_FILE} )
    configure_file( ${ORCA_CMAKE_DIR}/service.cpp.in ${SERVICE_CPP_FILE} )
    configure_file( ${ORCA_CMAKE_DIR}/component.h.in ${COMPONENT_H_FILE} )
    configure_file( ${ORCA_CMAKE_DIR}/component.cpp.in ${COMPONENT_CPP_FILE} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].in files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${SERVICE_CPP_FILE} ${COMPONENT_CPP_FILE} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

endmacro( ORCA_WRAP_MAINTHREAD_INTO_SERVICE SERVICE_NAME COMP_NAMESPACE COMP_NAME )
