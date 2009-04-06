include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

add_executable( privateutilstest privateutilstest.cpp )
GBX_ADD_TEST( OrcaIceDetail_PrivateUtils privateutilstest )

add_executable( propfiletest propfiletest.cpp )
GBX_ADD_TEST( OrcaIceDetail_PropFile propfiletest )

add_executable( homeregistrationtest homeregistrationtest.cpp )
GBX_ADD_TEST( OrcaIceDetail_HomeRegistration homeregistrationtest --Orca.Component.PrintProperties=1 )

