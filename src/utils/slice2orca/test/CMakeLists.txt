# simply run the utility with a special file which includes every possible Slice construct
# that we care about.
set( proj_slice_src_dir ${PROJECT_SOURCE_DIR}/src/interfaces/slice )

set( output "util" )
GBX_ADD_TEST( Slice2Orca_RunTestString 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2orca
        --${output}
        -I${proj_slice_src_dir} -I${ICE_SLICE_DIR}
        ${proj_slice_src_dir}/orca/test.ice  )

set( output "log" )
GBX_ADD_TEST( Slice2Orca_RunTestLog 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2orca 
        --${output}
        -I${proj_slice_src_dir} -I${ICE_SLICE_DIR}
        ${proj_slice_src_dir}/orca/test.ice  )

#
# Use the generated files
#
include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

add_executable( slice2orcastringtest stringtest.cpp )
target_link_libraries( slice2orcastringtest OrcaIfaceUtil )
GBX_ADD_TEST( Slice2Util_StringTest slice2orcastringtest )

add_executable( slice2orcazerotest zeroandcleartest.cpp )
target_link_libraries( slice2orcazerotest OrcaIfaceUtil )
GBX_ADD_TEST( Slice2Util_ZeroAndClearTest slice2orcazerotest )

add_executable( slice2orcalogtest logtest.cpp )
target_link_libraries( slice2orcalogtest OrcaIfaceLog )
GBX_ADD_TEST( Slice2Util_LogTest slice2orcalogtest )
