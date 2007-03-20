INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# Currently this file is peak_can specfic. Should ideally be generalised!
# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpCan SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpCan )

#Specifiy the library directories ie for the peak drivers
LINK_DIRECTORIES( /usr/lib/ ) 

#Make sure that we link the libraries we need including the peak one!
LINK_LIBRARIES( OrcaSegwayRmpCan libpcan.so )

#Build the stand alone test app if needed!
IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
