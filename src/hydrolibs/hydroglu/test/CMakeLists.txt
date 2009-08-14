find_package( OpenGL )

find_package( Qt4 )
set( QT_USE_QTOPENGL 1 )
include( ${QT_USE_FILE} )

include( ${GEARBOX_USE_FILE} )
include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
include( ${HYDRO_CMAKE_DIR}/UseIceUtil.cmake )
include( ${HYDRO_CMAKE_DIR}/UseGLEW.cmake )

link_libraries( HydroGlu GbxUtilAcfr ${OPENGL_LIBRARIES} ${QT_LIBRARIES} )

add_executable( testglsl testglsl.cpp )
# AlexB: Not running as an automated test because it requires (but
# doesn't check for) an X server.
#GBX_ADD_TEST( HydroGlu_Glsl testglsl )

add_executable( testtexture testtexture.cpp )
# AlexB: Not running as an automated test because it requires (but
# doesn't check for) an X server.
#GBX_ADD_TEST( HydroGlu_GlslTexture testtexture )

add_executable( testframebuffer testframebuffer.cpp )
