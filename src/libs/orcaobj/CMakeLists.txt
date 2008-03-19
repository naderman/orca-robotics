SET ( lib_name OrcaObjects )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
# This is one of fundamental libs. It is not optional (but maybe it should change)

SET ( ext_libs GbxUtilAcfr GbxIceUtilAcfr HydroUtil )
REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

IF ( build )

    # Do not include UseComponentRules here because it will link to OrcaIce
    # creating circular dependency.
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseGearbox.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    # for some reason cannot get shared libs working in windows
    IF(NOT ORCA_OS_WIN)
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE (NOT ORCA_OS_WIN)
    GLOBAL_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF (NOT ORCA_OS_WIN)
    
    TARGET_LINK_LIBRARIES( ${lib_name} OrcaInterfaces OrcaIce )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaInterfaces )
    
    INSTALL_FILES( /include/orcaobj .*\\.h$ )
    
    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF ( build )
