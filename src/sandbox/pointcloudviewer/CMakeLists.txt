OPTION( BUILD_POINT_CLOUD_VIEWER "Try to build point cloud viewer" OFF )

IF ( BUILD_POINT_CLOUD_VIEWER  )
    IF ( QT4_FOUND )

        INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
 	SET ( MOC_HEADERS 
		pcviewer.h
	)

	SET ( HUMAN_SRCS 
		pointcloudconsumerI
		pcviewer
	)
    	# Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    	INCLUDE_DIRECTORIES ( ${QT_INCLUDES} ) 

	QT4_WRAP_CPP ( MOC_SRCS ${MOC_HEADERS} )
    	# this driver can always be built
    	MESSAGE( STATUS "Point cloud viewer can be built" )
    
    	# stand-alone component  
    	GLOBAL_ADD_EXECUTABLE( pointcloudviewer main ${MOC_SRCS} ${HUMAN_SRCS} )
        TARGET_LINK_LIBRARIES ( pointcloudviewer ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY})

    ELSE ( QT4_FOUND )
	MESSAGE ( STATUS "Could NOT build pointcloudviewer as QT4 or GLUT was not found"  ) 
   ENDIF( QT4_FOUND )
ENDIF ( BUILD_POINT_CLOUD_VIEWER )
