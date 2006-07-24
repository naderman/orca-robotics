# for consistency with satellite projects, this CMakeLists.txt file is
# stored in [ORCA2]/config

IF ( JAVA_FOUND AND BUILD_JAVA )

    INCLUDE ( ${PROJECT_SOURCE_DIR}/config/build_interface_lib_java.cmake )

ENDIF ( JAVA_FOUND AND BUILD_JAVA )
