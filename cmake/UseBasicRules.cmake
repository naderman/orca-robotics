INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/libs
)

#
# Platform-specific compiler and linker flags
#
IF( NOT ORCA_OS_WIN )
    ADD_DEFINITIONS( "-Wall" )
ELSE ( NOT ORCA_OS_WIN )
    ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
ENDIF( NOT ORCA_OS_WIN )
