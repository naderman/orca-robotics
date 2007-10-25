INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

FILE( GLOB SRCS *.cpp )

IF(NOT OS_WIN)
    GLOBAL_ADD_LIBRARY( OrcaObjectUtil SHARED ${SRCS} )
ELSE (NOT OS_WIN)
    # for some reason cannot get shared libs working in windows
    GLOBAL_ADD_LIBRARY( OrcaObjectUtil STATIC ${SRCS} )
ENDIF (NOT OS_WIN)

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaObjectUtil OrcaObjects )

INSTALL_FILES( /include/orcaobjutil .*\\.h$ )
