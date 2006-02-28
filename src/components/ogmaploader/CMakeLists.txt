OPTION( BUILD_OGMAPLOADER "Try to build OgMapLoader component" ON )

SET( REALLY_BUILD_OGMAPLOADER ${BUILD_OGMAPLOADER} )

PKGCONFIG("gtk+-2.0" HAVE_GTK GTK_INCLUDE_DIRS GTK_DEFINES GTK_LINK_DIRS GTK_LIBS)
PKGCONFIG("gdk-pixbuf-2.0" HAVE_GDK_PIXBUF GDK_PIXBUF_INCLUDE_DIRS GDK_PIXBUF_DEFINES GDK_PIXBUF_LINK_DIRS GDK_PIXBUF_LIBS)

IF ( NOT HAVE_ZLIB )
  MESSAGE( "Not building OgMapLoader: zlib not found." )
  SET( REALLY_BUILD_OGMAPLOADER FALSE )
ENDIF ( NOT HAVE_ZLIB )
IF ( NOT HAVE_GDK_PIXBUF )
  MESSAGE( "Not building OgMapLoader: gdk-pixbuf not found." )
  SET( REALLY_BUILD_OGMAPLOADER FALSE )
ENDIF ( NOT HAVE_GDK_PIXBUF )
IF ( NOT HAVE_GTK )
  MESSAGE( "Not building OgMapLoader: gtk not found." )
  SET( REALLY_BUILD_OGMAPLOADER FALSE )
ENDIF ( NOT HAVE_GTK )


IF ( REALLY_BUILD_OGMAPLOADER )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

    INCLUDE_DIRECTORIES(
      ${GTK_INCLUDE_DIRS}
      ${GDK_PIXBUF_INCLUDE_DIRS}
      )
    
    LINK_LIBRARIES( ${GDK_PIXBUF_LIBS} z )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    FILE( GLOB SRCS *.cpp )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( ogmaploader ${SRCS} )

    GENERATE_FROM_DEF( ogmaploader.def )

ENDIF ( REALLY_BUILD_OGMAPLOADER )
