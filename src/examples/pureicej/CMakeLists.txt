option( build_PURE_ICEJ "Try to build PureIceJ Java Example" ON )

if( NOT GBX_OS_WIN AND build_PURE_ICEJ )

    PROJECT( pureicej Java )

    # TEMP HACKS
    set( PROJECT_INTERFACE_JAR ${orca_BINARY_DIR}/src/interfaces/java/orca.jar )

    # because we are in a separate project we can't refer to the Orca.jar target just by
    # its name, have to give the full path.
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.class
        COMMAND ${CMAKE_Java_COMPILER} 
        ARGS -classpath ${CMAKE_CURRENT_BINARY_DIR}:${PROJECT_INTERFACE_JAR}:${ICEJ_HOME}/Ice.jar
             -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/PureIceJ.java
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/PureIceJ.java
        DEPENDS ${PROJECT_INTERFACE_JAR}
        COMMENT "-- Building Java object ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.class" )

    message( STATUS "DEBUG: ${CMAKE_Java_COMPILER} -classpath ${CMAKE_CURRENT_BINARY_DIR} -classpath ${PROJECT_INTERFACE_JAR} -classpath ${ICEJ_HOME}/Ice.jar -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/PureIceJ.java" )
    
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.jar
        COMMAND ${CMAKE_COMMAND}
        ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_Java_ARCHIVE} -cf PureIceJ.jar *.class
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.class
        COMMENT "-- Creating archive ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.jar" )
    
    ADD_CUSTOM_TARGET( PureIceJ ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/PureIceJ.jar)

    GBX_ADD_ITEM( PureIceJ.jar )

endif( NOT GBX_OS_WIN AND build_PURE_ICEJ )
