# simply run the utility with a special file which includes every possible Slice construct
# that we care about.
SET( proj_slice_src_dir ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
SET( output "string" )

GBX_ADD_TEST( Slice2LogRunTest 
    ${CMAKE_CURRENT_BINARY_DIR}/../slice2log 
        --${output}
        -I${proj_slice_src_dir} 
        ${proj_slice_src_dir}/orca/test.ice  )

#
# Use the generated files
#
INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceLog.cmake )

ADD_EXECUTABLE( logstreamtest logtest.cpp )
GBX_ADD_TEST( Slice2LogLogStreamTest logstreamtest )
