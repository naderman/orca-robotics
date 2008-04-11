SET( lib_name OrcaQCompModel )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} NOT_GBX_OS_WIN "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaCompModel )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

SET( dep_libs ${dep_libs} IceGrid )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )

    SET( moc_hdrs
      ocmmodel.h
      ocmview.h
      ocmdelegate.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} ${moc_srcs} )
    
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )
    
    GBX_ADD_HEADERS( orcaqcm ${hdrs} )

ENDIF( build )
