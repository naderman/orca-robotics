if( JAVA_FOUND AND ORCA_BUILD_JAVA )

message( STATUS "Planning to build Java interfaces." )

project( java_interfaces Java )
# set( CMAKE_VERBOSE_MAKEFILE 1 )

# TEMP HACKS
set( PROJECT_INTERFACE_JAR orca.jar )
set( PROJECT_INTERFACE_NAMESPACE orca )
set( PROJECT_INTERFACE_TOKEN_FILE common.ice )
set( PROJECT_INTERFACE_TOKEN_CLASS Time )

set( SLICE2JAVA_COMMAND        ${ICE_HOME}/bin/slice2java${EXE_EXTENSION} )

set( SLICE_SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/../slice )
set( SLICE_BINARY_DIR          ${CMAKE_CURRENT_BINARY_DIR}/../slice )
set( SLICE2JAVA_BINARY_DIR     ${CMAKE_CURRENT_BINARY_DIR} )

# debian package splits off slice files into a different place
if( ICE_HOME MATCHES /usr )
    set( ice_slice_dir /usr/share/slice )
    message( STATUS "This is a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
else( ICE_HOME MATCHES /usr )
    set( ice_slice_dir ${ICE_HOME}/slice )
    message( STATUS "This is NOT a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
endif( ICE_HOME MATCHES /usr )

# note: compared to slice2cpp, slice2java automatically places generated files into 'namespace dir,
#       e.g. 'orca'. So the output dir is just 'java' not 'java/<namespace>'
set( SLICE_ARGS -I${SLICE_SOURCE_DIR} -I${ice_slice_dir} --stream --output-dir ${SLICE2JAVA_BINARY_DIR} --meta "java:java5" )
# note: satelite projects need to include slice files from orca installation
if( DEFINED ORCA_HOME )
    set( SLICE_ARGS -I${ORCA_HOME}/slice ${SLICE_ARGS} )
endif( DEFINED ORCA_HOME )

# ALL .java files for the .class files
add_custom_command(
    OUTPUT ${SLICE2JAVA_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.java
    COMMAND ${SLICE2JAVA_COMMAND} 
    ARGS ${SLICE_SOURCE_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.ice ${SLICE_ARGS}
    MAIN_DEPENDENCY ${SLICE_SOURCE_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_FILE}
    COMMENT "-- Generating .java files from .ice sources." )

message( STATUS "Will generate .java files from Slice definitions using this command:" )
message( STATUS "${SLICE2JAVA_COMMAND} *.ice ${SLICE_ARGS}" )

# ALL .class files for the .jar file
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.class
    COMMAND ${CMAKE_Java_COMPILER} 
    ARGS -classpath ${CMAKE_CURRENT_SOURCE_DIR} -classpath ${ICEJ_HOME}/Ice.jar -d ${CMAKE_CURRENT_BINARY_DIR} 
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.java
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.java
    COMMENT "-- Building Java objects ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}/*.class" )

message( STATUS "DEBUG: Will build Java objects using this command:" )
message( STATUS "${CMAKE_Java_COMPILER} -classpath ${CMAKE_CURRENT_SOURCE_DIR} -classpath ${ICEJ_HOME}/Ice.jar -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.java" )

# the .jar file for the target
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_JAR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/${PROJECT_INTERFACE_TOKEN_CLASS}.class
    COMMAND ${CMAKE_COMMAND}
    ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_Java_ARCHIVE} 
        -cf ${PROJECT_INTERFACE_JAR} ${PROJECT_INTERFACE_NAMESPACE}/*.class
#         -cf ${PROJECT_INTERFACE_JAR} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_NAMESPACE}/*.class
    COMMENT "-- Creating archive ${PROJECT_INTERFACE_JAR} from .class files." )

# the target
ADD_CUSTOM_TARGET( ${PROJECT_INTERFACE_JAR} ALL 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_INTERFACE_JAR})

GBX_ADD_ITEM( ${PROJECT_INTERFACE_JAR} )
INSTALL_FILES( ${GBX_LIB_INSTALL_DIR} FILES ${PROJECT_INTERFACE_JAR} )
message( STATUS "Will install archive ${PROJECT_INTERFACE_JAR} into ${GBX_LIB_INSTALL_DIR}" )

else( JAVA_FOUND AND ORCA_BUILD_JAVA )
    message( STATUS "Will not build Java interfaces : JAVA_FOUND=${JAVA_FOUND}, ORCA_BUILD_JAVA=${ORCA_BUILD_JAVA}" )
endif( JAVA_FOUND AND ORCA_BUILD_JAVA )
