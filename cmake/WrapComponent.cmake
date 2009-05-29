#
# ORCA_WRAP_COMPONENT_INTO_APP( SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# The following CMake variables must be defined, e.g. by calling ORCA_SET_COMPONENT_NAME() 
# - APP_NAME 
# - APP_CTRLC_HANDLER 
# - COMP_NAMESPACE
# 
# In addition this macro expects to find in the current source directory: 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
macro( ORCA_WRAP_COMPONENT_INTO_APP )

    # here and below: we could ensure here that component.h/cpp exist but I'm not sure
    # the extra overhead is needed since compiler will say exactly that 2 min later.

    if( NOT DEFINED APP_NAME )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable APP_NAME is not defined" )
    endif()
    if( NOT DEFINED APP_CTRLC_HANDLER )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable APP_CTRLC_HANDLER is not defined" )
    endif()
    if( NOT DEFINED COMP_NAMESPACE )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_NAMESPACE is not defined" )
    endif()

    set( _main_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/main.cpp.in ${_main_cpp_file} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${_main_cpp_file} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

endmacro( ORCA_WRAP_COMPONENT_INTO_APP )

#
# ORCA_WRAP_COMPONENT_INTO_SERVICE( SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# The following CMake variables must be defined, e.g. by calling ORCA_SET_COMPONENT_NAME() 
# - SERVICE_NAME 
# - COMP_NAMESPACE
# 
# In addition this macro expects to find in the current source directory: 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
macro( ORCA_WRAP_COMPONENT_INTO_SERVICE )

    if( NOT DEFINED SERVICE_NAME )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable SERVICE_NAME is not defined" )
    endif()
    if( NOT DEFINED COMP_NAMESPACE )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_NAMESPACE is not defined" )
    endif()

    set( _service_h_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    set( _service_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/service.h.in ${_service_h_file} )
    configure_file( ${ORCA_CMAKE_DIR}/service.cpp.in ${_service_cpp_file} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].in files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${_service_cpp_file} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

endmacro( ORCA_WRAP_COMPONENT_INTO_SERVICE )

#
# ORCA_WRAP_MAINTHREAD_INTO_APP( SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# The following CMake variables must be defined, e.g. by calling ORCA_SET_COMPONENT_NAME() 
# - APP_NAME 
# - APP_CTRLC_HANDLER 
# - COMP_NAMESPACE
# - COMP_NAME
# 
# In addition this macro expects to find in the current source directory: 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
macro( ORCA_WRAP_MAINTHREAD_INTO_APP )

    if( NOT DEFINED APP_NAME )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable APP_NAME is not defined" )
    endif()
    if( NOT DEFINED APP_CTRLC_HANDLER )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable APP_CTRLC_HANDLER is not defined" )
    endif()
    if( NOT DEFINED COMP_NAMESPACE )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_NAMESPACE is not defined" )
    endif()
    if( NOT DEFINED COMP_NAME )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_NAME is not defined" )
    endif()
    if( NOT DEFINED COMP_INTERFACE_FLAG )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_INTERFACE_FLAG is not defined" )
    endif()
    if( NOT DEFINED COMP_ADAPTER_POLICY )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_ADAPTER_POLICY is not defined" )
    endif()

    set( _main_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/main.cpp )
    set( _component_h_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    set( _component_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/main.cpp.in ${_main_cpp_file} )
    configure_file( ${ORCA_CMAKE_DIR}/component.h.in ${_component_h_file} )
    configure_file( ${ORCA_CMAKE_DIR}/component.cpp.in ${_component_cpp_file} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${ARGN} ${_main_cpp_file} ${_component_cpp_file} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

endmacro( ORCA_WRAP_MAINTHREAD_INTO_APP )


#
# ORCA_WRAP_MAINTHREAD_INTO_SERVICE( SOURCE0 [SOURCE1 SOURCE2 ...] )
#
# The following CMake variables must be defined, e.g. by calling ORCA_SET_COMPONENT_NAME() 
# - SERVICE_NAME 
# - COMP_NAMESPACE
# - COMP_NAME
# 
# In addition this macro expects to find in the current source directory: 
# - file mainthread.h/cpp 
# - which defines class ${COMP_NAMESPACE}::MainThread
# - which derives from hydroiceutil::Thread or its derivatives
#
macro( ORCA_WRAP_MAINTHREAD_INTO_SERVICE )

    if( NOT DEFINED COMP_NAME )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_NAME is not defined" )
    endif()
    if( NOT DEFINED COMP_NAMESPACE )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_NAMESPACE is not defined" )
    endif()
    if( NOT DEFINED SERVICE_NAME )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable SERVICE_NAME is not defined" )
    endif()
    if( NOT DEFINED COMP_INTERFACE_FLAG )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_INTERFACE_FLAG is not defined" )
    endif()
    if( NOT DEFINED COMP_ADAPTER_POLICY )
        message( FATAL_ERROR "(while wrapping main thread into app) CMake variable COMP_ADAPTER_POLICY is not defined" )
    endif()

    set( _service_h_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.h )
    set( _service_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/service.cpp )
    set( _component_h_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.h )
    set( _component_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/autogen/component.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/service.h.in ${_service_h_file} )
    configure_file( ${ORCA_CMAKE_DIR}/service.cpp.in ${_service_cpp_file} )
    configure_file( ${ORCA_CMAKE_DIR}/component.h.in ${_component_h_file} )
    configure_file( ${ORCA_CMAKE_DIR}/component.cpp.in ${_component_cpp_file} )

    # This works in Linux.
    # It's easier to compile STATIC lib in Windows. But I'm not sure if IceBox
    # would be happy with a static lib. So we should figure out how to do shared
    # libs properly in Windows. Hopefully, the changes would be confined to
    # to service.[h/cpp].in files.
    # So for now, NOT using DEFAULT option here!
    GBX_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${ARGN} ${_service_cpp_file} ${_component_cpp_file} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/autogen )

endmacro( ORCA_WRAP_MAINTHREAD_INTO_SERVICE )
