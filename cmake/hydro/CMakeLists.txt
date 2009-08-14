#
# Install all .cmake files, so other projects can use them.
#
file( GLOB scripts *.cmake *.template )
file( GLOB cvcameratest opencvcameratest/*.cpp )
GBX_ADD_SHARED_FILES( cmake ${scripts} )
GBX_ADD_SHARED_FILES( cmake/opencvcameratest ${cvcameratest} )
