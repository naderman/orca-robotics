#
# This file doesn't actually build the ice files.  
# That's done in src/interfaces/<language>/orca/CMakeLists.txt,
# using the list of .ice files given in slice_sources.cmake.
#


#
# Install all the .ice files, so other projects can use them.
# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the Slice module, e.g. 'orca'
#
GET_FILENAME_COMPONENT( slice_module ${CMAKE_CURRENT_SOURCE_DIR} NAME )

FILE( GLOB slices *.ice )
GBX_ADD_SHARED_FILES( slice/${slice_module} ${slices} )
