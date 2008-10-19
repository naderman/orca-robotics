INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

ADD_EXECUTABLE( ifacetest ifacetest.cpp )
TARGET_LINK_LIBRARIES( ifacetest OrcaIfaceImpl )
GBX_ADD_TEST( OrcaIfaceImplTest ifacetest )

ADD_EXECUTABLE( consumerimpltest consumerimpltest.cpp )
TARGET_LINK_LIBRARIES( consumerimpltest OrcaIfaceImpl )
GBX_ADD_TEST( OrcaIfaceImplConsumerTest consumerimpltest )
