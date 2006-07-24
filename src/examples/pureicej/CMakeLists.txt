OPTION( BUILD_PURE_ICEJ "Try to build PureIceJ Java Example" ON )

IF ( NOT WIN32 AND BUILD_PURE_ICEJ )

    PROJECT( pureicej Java )

    # TEMP HACKS
    SET ( ICEJ_HOME /opt/IceJ-3.1.0-java2 )
    SET ( PROJECT_INTERFACE_JAR ${orca_BINARY_DIR}/src/interfaces/java/orca.jar )

    # because we are in a separate project we can't refer to the Orca.jar target just by
    # its name, have to give the full path.
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.class
        COMMAND ${CMAKE_Java_COMPILER} 
        ARGS -cp ${CMAKE_CURRENT_BINARY_DIR}:${PROJECT_INTERFACE_JAR}:${ICEJ_HOME}/lib/Ice.jar
#         ARGS -cp ${CMAKE_CURRENT_BINARY_DIR} -cp ${ICEJ_HOME}/lib/Ice.jar 
#         ARGS -cp ${CMAKE_CURRENT_BINARY_DIR}
             -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/PureIceJ.java
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/PureIceJ.java
        DEPENDS ${PROJECT_INTERFACE_JAR}
        COMMENT "-- Building Java object ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.class" )

    MESSAGE ( STATUS "DEBUG: ${CMAKE_Java_COMPILER} -cp ${CMAKE_CURRENT_BINARY_DIR} -cp ${PROJECT_INTERFACE_JAR} -cp ${ICEJ_HOME}/lib/Ice.jar -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/PureIceJ.java" )
    
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.jar
        COMMAND ${CMAKE_COMMAND}
        ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_Java_ARCHIVE} -cf PureIceJ.jar *.class
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.class
        COMMENT "-- Creating archive ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.jar" )
    
    ADD_CUSTOM_TARGET( PureIceJ ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.jar)

    MESSAGE( STATUS "Planning to Build Executable: PureIceJ" )

ENDIF ( NOT WIN32 AND BUILD_PURE_ICEJ )
