#
# This file doesn't actually build the ice files.  
# That's done in src/interfaces/<language>/orca/CMakeLists.txt,
# using the list of .ice files given in slice_sources.cmake.
#


#
# Install all the .ice files, so other projects can use them.
#
INSTALL_FILES( /ice/orca .*\\.ice$ )
