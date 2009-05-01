#
# Install all the .cmake scripts and source templates, so other projects can use them.
#
file( GLOB scripts *.cmake *.in )
GBX_ADD_SHARED_FILES( cmake ${scripts} )
