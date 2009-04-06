# BUILD_COMPONENT_SOURCES
#   DEPRICATED!!!! Not portable!!!
#   Builds source files common to stand-alone component and IceStorm service.
#
#       BUILD_COMPONENT_SOURCES( CONV_LIB_NAME )
#   The common sources are compiled into a convenience static library with
#   specified name.
#
macro( BUILD_COMPONENT_SOURCES CONV_LIB_NAME )
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    file( GLOB COMMON_SRCS *.cpp )
    list( REMOVE_ITEM COMMON_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    # build common sources into a static "convenience" library
    add_library( ${CONV_LIB_NAME} ${COMMON_SRCS} )
    # special dependency on the generated files 
    # (each project should wait for its own interface lib)
    add_dependencies( ${CONV_LIB_NAME} ${PROJECT_INTERFACE_LIB} )
endmacro( BUILD_COMPONENT_SOURCES CONV_LIB_NAME )
