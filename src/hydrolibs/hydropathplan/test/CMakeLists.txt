include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

add_executable( testconversions testconversions.cpp )
target_link_libraries( testconversions HydroPathPlan  )
GBX_ADD_TEST( HydroPathPlanConversion testconversions )

set( exe_name testpathplan )
# this test has external dependencies
set( build TRUE )
# The map loading stuff relies on Qt
GBX_REQUIRE_VAR( build EXE ${exe_name} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_LIBS( build EXE ${exe_name} HydroMapLoad )

if( build )

    add_executable( testpathplan testpathplan.cpp )
    target_link_libraries( testpathplan HydroMapLoad HydroPathPlan )
    GBX_ADD_TEST( HydroPathPlan testpathplan ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )

endif( build )