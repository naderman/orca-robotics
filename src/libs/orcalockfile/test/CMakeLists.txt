  INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/libs
    )

GLOBAL_ADD_EXECUTABLE( locktest locktest.cpp )
TARGET_LINK_LIBRARIES( locktest OrcaLockFile )
