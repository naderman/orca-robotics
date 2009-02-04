INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

ADD_EXECUTABLE( privateutilstest privateutilstest.cpp )
GBX_ADD_TEST( OrcaIceDetail_PrivateUtils privateutilstest )

ADD_EXECUTABLE( propfiletest propfiletest.cpp )
GBX_ADD_TEST( OrcaIceDetail_PropFile propfiletest )

ADD_EXECUTABLE( homeregistrationtest homeregistrationtest.cpp )
GBX_ADD_TEST( OrcaIceDetail_HomeRegistration homeregistrationtest --Orca.Component.PrintProperties=1 )

