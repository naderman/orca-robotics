LINK_LIBRARIES( OrcaIfaceImpl )

# ADD_EXECUTABLE( ifacetest ifacetest.cpp )
# GBX_ADD_TEST( OrcaIfaceImpl_Test ifacetest )

ADD_EXECUTABLE( consumerimpltest consumerimpltest.cpp )
GBX_ADD_TEST( OrcaIfaceImpl_ConsumerTest consumerimpltest )

ADD_EXECUTABLE( providerimpltest providerimpltest.cpp )
GBX_ADD_TEST( OrcaIfaceImpl_ProviderTest providerimpltest )
