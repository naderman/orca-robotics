link_libraries( OrcaIfaceImpl )

# add_executable( ifacetest ifacetest.cpp )
# GBX_ADD_TEST( OrcaIfaceImpl_Test ifacetest )

add_executable( consumerimpltest consumerimpltest.cpp )
GBX_ADD_TEST( OrcaIfaceImpl_ConsumerTest consumerimpltest )

add_executable( providerimpltest providerimpltest.cpp )
GBX_ADD_TEST( OrcaIfaceImpl_ProviderTest providerimpltest )
