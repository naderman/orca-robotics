# A place for well-tested and well-documented libraries.

message( STATUS "== HYDRO LIBRARIES ==" )

# when adding new directories, please maintain the order of dependencies.

add_subdirectory( hydroutil )
add_subdirectory( hydroiceutil )
add_subdirectory( hydrodll )
add_subdirectory( hydroportability )

add_subdirectory( hydrobros1 )
add_subdirectory( hydrocolourtext )
add_subdirectory( hydrogeom2d )
add_subdirectory( hydrofeatureobs ) # requires hydrogeom2d
add_subdirectory( hydroglu )
add_subdirectory( hydrodisparity )
add_subdirectory( hydrogpsutil )
add_subdirectory( hydroimage )
add_subdirectory( hydroimageocvutil )
add_subdirectory( hydroogmap )
add_subdirectory( hydromapload ) # requires hydroogmap
add_subdirectory( hydronavutil )
add_subdirectory( hydrorobotdriverutil )
add_subdirectory( hydropathplan )
add_subdirectory( hydropublish )
add_subdirectory( hydrotime )
add_subdirectory( hydroscanutil ) # requires hydronavutil
add_subdirectory( hydrosim2d ) # requires hydronavutil
add_subdirectory( hydrolaserfeatureextract ) # tests require hydromapload and hydrosim2d
add_subdirectory( hydronovatelutil )
add_subdirectory( hydroping )
add_subdirectory( hydroqguielementutil )
add_subdirectory( hydroqgui ) # requires hydroqguielementutil
add_subdirectory( hydroqguipath )
add_subdirectory( hydrowifi )
