INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

ADD_EXECUTABLE( ifacetest ifacetest.cpp )
TARGET_LINK_LIBRARIES( ifacetest OrcaIfaceImpl )
GLOBAL_ADD_TEST( OrcaIfaceImplTest ifacetest )
