include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

if( buildCAN )
  add_definitions( -DCAN_IO_DRIVER )
  GBX_ADD_EXECUTABLE( hydrosegwayrmpacfrcan hydrosegwayrmpacfrcantest.cpp )
  target_link_libraries( hydrosegwayrmpacfrcan HydroSegwayRmpAcfrCan )
endif( buildCAN )
