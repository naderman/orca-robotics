# BUILD_COMPONENT_SOURCES
#   DEPRICATED!!!! Not portable!!!
#   Builds source files common to stand-alone component and IceStorm service.
#
#       BUILD_COMPONENT_SOURCES( CONV_LIB_NAME )
#   The common sources are compiled into a convenience static library with
#   specified name.
#
MACRO( BUILD_COMPONENT_SOURCES CONV_LIB_NAME )
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    FILE( GLOB COMMON_SRCS *.cpp )
    LIST( REMOVE_ITEM COMMON_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    # build common sources into a static "convenience" library
    ADD_LIBRARY ( ${CONV_LIB_NAME} ${COMMON_SRCS} )
    # special dependency on the generated files 
    # (each project should wait for its own interface lib)
    ADD_DEPENDENCIES( ${CONV_LIB_NAME} ${PROJECT_INTERFACE_LIB} )
ENDMACRO( BUILD_COMPONENT_SOURCES CONV_LIB_NAME )
