set( module_name def_tools )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${module_name} ON )

if( build )

    # needs defintions from orca/ocm.ice
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    link_libraries( OrcaDef )
    
    GBX_ADD_EXECUTABLE( def2cfg def2cfg.cpp )
    
    GBX_ADD_EXECUTABLE( def2xml def2xml.cpp )
    
    # it's not currently used.
    # generation of XML template is now rolled into def2xml as an option.
#     GBX_ADD_EXECUTABLE( def2xmltemplate def2xmltemplate.cpp )
    
    # it's not currently used.
    # it was intended for the Wall utility which could provide
    # a perfect compliment for any component's required interfaces.
#     GBX_ADD_EXECUTABLE( def2definverted def2definverted.cpp )

endif( build )
