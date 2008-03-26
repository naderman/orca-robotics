#
# Install all the .cmake scripts and source templates, so other projects can use them.
#
FILE( GLOB scripts *.cmake *.template )
GBX_ADD_SHARED_FILES( cmake ${scripts} )
