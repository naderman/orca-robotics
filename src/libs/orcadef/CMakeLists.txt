ADD_DEFINITIONS( "-Wall" )
INCLUDE_DIRECTORIES( 
    ${ICE_HOME}/include
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp 
    ${PROJECT_SOURCE_DIR}/src/libs
    )
LINK_DIRECTORIES( ${ICE_HOME}/lib )

# for some reason cannot get shared libs working in windows
IF(NOT OS_WIN)
  GLOBAL_ADD_LIBRARY( OrcaDef SHARED defparser.cpp componentdefinition.cpp exceptions.cpp )
ELSE (NOT OS_WIN)
  GLOBAL_ADD_LIBRARY( OrcaDef STATIC defparser.cpp componentdefinition.cpp exceptions.cpp )
ENDIF (NOT OS_WIN)

TARGET_LINK_LIBRARIES( OrcaDef OrcaInterfaces OrcaIce OrcaIceUtil OrcaObjects Ice IceUtil )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaDef OrcaObjects OrcaIce )

INSTALL_FILES( /include/orcadef .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
