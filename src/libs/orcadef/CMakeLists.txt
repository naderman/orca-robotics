INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# for some reason cannot get shared libs working in windows
IF(NOT WIN32)
  GLOBAL_ADD_LIBRARY( OrcaDef SHARED defparser.cpp componentdefinition.cpp exceptions.cpp )
ELSE (NOT WIN32)
  GLOBAL_ADD_LIBRARY( OrcaDef STATIC defparser.cpp componentdefinition.cpp exceptions.cpp )
ENDIF (NOT WIN32)

TARGET_LINK_LIBRARIES( OrcaDef OrcaObjects OrcaIce )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaDef OrcaObjects OrcaIce )

INSTALL_FILES( /include/orcadef .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
