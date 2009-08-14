#
# HYDRO_ADD_LINK_TEST ( interface_name driver_namespace driver_header LIB0 [LIB1 LIB2 ...] )
#
# Expects to find 
# - file component.h/cpp 
# - which defines class ${COMP_NAMESPACE}::Component
# - which derives from orcaice::Component
#
macro( HYDRO_ADD_LINK_TEST interface_name driver_namespace driver_header )

    set( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/linktest.cpp )

    set( interface_name ${interface_name} ) 
    set( factory_name ${interface_name}Factory ) 
    set( driver_namespace ${driver_namespace} ) 
    set( driver_header ${driver_header} )

    set( target_name ${driver_namespace}_linktest )

    configure_file( ${ORCA_CMAKE_DIR}/linktest.cpp.template ${MAIN_CPP_FILE} )

    add_executable( ${target_name} ${MAIN_CPP_FILE} )
    target_link_libraries( ${target_name} ${ARGN} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

endmacro( HYDRO_ADD_LINK_TEST interface_name driver_namespace driver_header )


macro( HYDRO_ADD_LINK_TEST_WITH_CONFIG interface_name driver_namespace driver_header )

    set( MAIN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/autogen/linktest.cpp )

    set( interface_name ${interface_name} ) 
    set( factory_name ${interface_name}Factory ) 
    set( driver_namespace ${driver_namespace} ) 
    set( driver_header ${driver_header} )

    set( target_name ${driver_namespace}_linktest )

    configure_file( ${ORCA_CMAKE_DIR}/linktestwithconfig.cpp.template ${MAIN_CPP_FILE} )

    add_executable( ${target_name} ${MAIN_CPP_FILE} )
    target_link_libraries( ${target_name} ${ARGN} )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

endmacro( HYDRO_ADD_LINK_TEST_WITH_CONFIG interface_name driver_namespace driver_header )
