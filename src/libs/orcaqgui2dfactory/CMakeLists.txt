ORCA_SET_LIBRARY_NAME( OrcaQGui2dFactory )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

set( dep_libs HydroNavUtil HydroQGui HydroQGuiPath HydroQGuiElementUtil HydroOgMap 
              OrcaObjects OrcaObjectUtil OrcaQGuiElementUtil OrcaOgMap )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )
    
    include( ${HYDRO_USE_FILE} )
    include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB human_srcs *.cpp )
    
    set( moc_hdrs
        pathfolloweruserinteraction.h
        pathplanneruserinteraction.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${human_srcs} ${moc_srcs} )
    
    link_directories( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    GBX_ADD_SHARED_FILES( ${LIB_INSTALL} ogHazardLookupTable.png )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )
    
endif( build )
