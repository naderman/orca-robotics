GBX_ADD_LICENSE( LGPL )

# this driver is special: it has two options for IO.
# for this reason, this file is more complicated than usual.
# don't use it as a template for your new driver, unless you have to.
set( buildCAN false )
set( buildUSB false )

#
# CAN IO
#
ORCA_SET_LIBRARY_NAME( HydroSegwayRmpAcfrCan )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "Only Linux OS is supported" )

set( dep_libs ${dep_libs} GbxUtilAcfr GbxIceUtilAcfr HydroInterfaces HydroIceUtil ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

include(${ORCA_CMAKE_DIR}/FindPeakCan.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} PEAKCAN_FOUND "libpcan (Peak CAN) was not found" )
set( dep_libs ${dep_libs} pcan )

if( build )

    set( buildCAN true )
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp canio/*.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )
    # this flag tells the driver which IO mechanism to use for this target
    # (not using add_definitions() command because it sets flags for the whole directory)
    set_target_properties( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -DCAN_IO_DRIVER )
    # for test utility only
    add_subdirectory( canio )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( testcan )
    endif( ORCA_BUILD_TESTS )

    GBX_ADD_HEADERS( segwayrmpacfr ${hdrs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST( SegwayRmp segwayrmpacfr driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif( build )

#
#  USB IO
#
ORCA_SET_LIBRARY_NAME( HydroSegwayRmpAcfrUsb )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "Only Linux OS is supported" )

set( dep_libs GbxUtilAcfr GbxIceUtilAcfr HydroInterfaces HydroIceUtil ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

include( ${ORCA_CMAKE_DIR}/FindFtd2xx.cmake )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} FTD2XX_FOUND "libftd2xx was not found" )
set( dep_libs ${dep_libs} ftd2xx usb )

if( build )

    set( buildUSB true )
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp usbio/*.cpp )

    # special case: the FTDI driver is implemented as a separate lib
    add_subdirectory( usbio )
    set( dep_libs ${dep_libs} HydroSegwayRmpAcfrUsbFtdi )

    # this flag tells the driver which IO mechanism to use
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )
    # this flag tells the driver which IO mechanism to use for this target
    # (not using add_definitions() command because it sets flags for the whole directory)
    set_target_properties( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -DUSB_IO_DRIVER )

    GBX_ADD_HEADERS( segwayrmpacfr ${hdrs} )

endif( build )


#
# generic tests/utilities
#
if( buildCAN OR buildUSB )
  add_subdirectory( util )
endif( buildCAN OR buildUSB )
