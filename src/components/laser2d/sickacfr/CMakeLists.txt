IF ( NOT OS_LINUX )
  MESSAGE( STATUS "Not building OrcaLaser2dSickAcfr -- not Linux." )
ELSE ( NOT OS_LINUX )

FILE( GLOB SRCS *.cpp )

# Remove test source file
LIST( REMOVE_ITEM SRCS 
  "${CMAKE_CURRENT_SOURCE_DIR}/laser2dsickacfr.cpp" )
  
ADD_LIBRARY( OrcaLaser2dSickAcfr SHARED ${SRCS} )
TARGET_LINK_LIBRARIES( OrcaLaser2dSickAcfr OrcaLaser2dUtil OrcaSerial )

INSTALL_TARGETS( /lib OrcaLaser2dSickAcfr )

# Test for correct linking.
ADD_EXECUTABLE( laser2dsickacfr laser2dsickacfr.cpp )
TARGET_LINK_LIBRARIES( laser2dsickacfr OrcaLaser2dSickAcfr )

ENDIF ( NOT OS_LINUX )