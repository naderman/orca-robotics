# simply run the utility with a special file which includes every possible Slice construct
# that we care about.
SET( proj_slice_src_dir ${PROJECT_SOURCE_DIR}/src/interfaces/slice )

SET( output "string" )
GBX_ADD_TEST( Slice2Orca_RunTestString 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2orca
        --${output}
        -I${proj_slice_src_dir} -I${ICE_SLICE_DIR}
        ${proj_slice_src_dir}/orca/test.ice  )

SET( output "log" )
GBX_ADD_TEST( Slice2Orca_RunTestLog 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2orca 
        --${output}
        -I${proj_slice_src_dir} -I${ICE_SLICE_DIR}
        ${proj_slice_src_dir}/orca/test.ice  )

#
# Use the generated files
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

ADD_EXECUTABLE( slice2orcastringtest stringtest.cpp )
TARGET_LINK_LIBRARIES( slice2orcastringtest OrcaIfaceUtil )
GBX_ADD_TEST( Slice2Util_StringTest slice2orcastringtest )

ADD_EXECUTABLE( slice2orcazerotest zeroandcleartest.cpp )
TARGET_LINK_LIBRARIES( slice2orcazerotest OrcaIfaceUtil )
GBX_ADD_TEST( Slice2Util_ZeroAndClearTest slice2orcazerotest )

ADD_EXECUTABLE( slice2orcalogtest logtest.cpp )
TARGET_LINK_LIBRARIES( slice2orcalogtest OrcaIfaceLog )
GBX_ADD_TEST( Slice2Util_LogTest slice2orcalogtest )
