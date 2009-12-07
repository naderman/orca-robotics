ORCA_SET_LIBRARY_NAME( OrcaLogFactory )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )

set( dep_libs OrcaLog OrcaIfaceImpl OrcaObjects OrcaObjectUtil OrcaIfaceLog )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # Image/Camera log recorder/player are special:
    #    they have an external dependency on OpenCV
    # In odrer to optionally avoid this dependency, we create a special
    # user-configurable cmake variable ORCALOGFACTORY_USE_OPENCV
    option( ORCALOGFACTORY_USE_OPENCV "If OpenCV is installed, should libOrcaLogFactory use it?" ON )
    if ( ORCALOGFACTORY_USE_OPENCV )
        include( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
        if( OPENCV_FOUND )
            set( dep_libs_ocv HydroDll HydroImage HydroImageOcvUtil )

	    set( build_with_ocv TRUE )
	    GBX_REQUIRE_LIBS( build_with_ocv LIB "${LIB_NAME} OpenCV support" ${dep_libs_ocv} )

	    if( build_with_ocv )

                include( ${ORCA_CMAKE_DIR}/UseOpenCV.cmake )
                add_definitions( -DOPENCV_FOUND )
	        set( dep_libs ${dep_libs} ${dep_libs_ocv} )
		message( STATUS "libOrcaLogFactory is using OpenCV" )

	    else( build_with_ocv )

	        message( STATUS "libOrcaLogFactory not using OpenCV due to dependencies" )

	    endif( build_with_ocv )
        endif( OPENCV_FOUND )
    endif( ORCALOGFACTORY_USE_OPENCV )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )

    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )
