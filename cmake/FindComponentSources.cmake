# FIND_COMPONENT_SOURCES
#   Returns a list of source files except main.cpp and service.cpp
#
#       FIND_COMPONENT_SOURCES( VAR_NAME )
#
MACRO( FIND_COMPONENT_SOURCES VAR_NAME )
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    FILE( GLOB COMMON_SRCS *.cpp )
    LIST( REMOVE_ITEM COMMON_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    SET( ${VAR_NAME} ${COMMON_SRCS} )
ENDMACRO( FIND_COMPONENT_SOURCES VAR_NAME )
