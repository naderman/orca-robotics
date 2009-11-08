ORCA_SET_LIBRARY_NAME( OrcaPublish )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs OrcaIfaceImpl OrcaScanUtil OrcaNavUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

  include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
  include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
  include( ${GEARBOX_USE_FILE} )

  file( GLOB srcs *.cpp )
    
  GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${LIB_VERSION} ${srcs} )
  target_link_libraries( ${LIB_NAME} ${dep_libs} )

endif( build )