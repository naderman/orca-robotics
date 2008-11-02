# simply run the utility with a special file which includes every possible Slice construct
# that we care about.
SET( proj_slice_src_dir ${PROJECT_SOURCE_DIR}/src/interfaces/slice )

SET( output "string" )
GBX_ADD_TEST( Slice2LogRunTestString 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2log 
        --${output}
        -I${proj_slice_src_dir} -I${ICE_SLICE_DIR}
        ${proj_slice_src_dir}/orca/test.ice  )

SET( output "log" )
GBX_ADD_TEST( Slice2LogRunTestLog 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2log 
        --${output}
        -I${proj_slice_src_dir} -I${ICE_SLICE_DIR}
        ${proj_slice_src_dir}/orca/test.ice  )

#
# Use the generated files
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceLog.cmake )

ADD_EXECUTABLE( slice2utilstringtest stringtest.cpp )
GBX_ADD_TEST( Slice2UtilStringTest slice2utilstringtest )

ADD_EXECUTABLE( slice2utillogtest logtest.cpp )
GBX_ADD_TEST( Slice2UtilLogTest slice2utillogtest )
