# include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# Glob all source files
file( GLOB srcs *.cpp )

# include_directories( ${PROJECT_SOURCE_DIR}/src/components/segwayrmp/rmpdriver/usb )

GBX_ADD_LIBRARY( HydroSegwayRmpAcfrUsbFtdi DEFAULT ${LIB_VERSION} ${srcs} )
