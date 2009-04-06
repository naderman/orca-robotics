#
# Macro to check for optional sub-libraries, eg in the case where
# a single component can drive various bits of hardware with the 
# same interface.
#
# It's probably just as easy to write this stuff by hand, but
# using a macro standardizes the trace statements.
#
# USAGE: ORCA_OPTIONAL_SUB_LIBRARY( 
#                   description 
#                   subdir 
#                   output_library 
#                   link_libs
#                   ok_to_build 
#                   definition_tag
#                   lib1 lib2 ... libn )
#
# Where:
#  - description:       Descriptive message
#  - subdir:            Where the code lives
#  - output_library:    The optional sub-library
#  - link_libs:         Extra libraries one needs to link against
#  - ok_to_build:       The name of the variable that holds whether this sub-library be build
#  - definition_tag:    String tag used for both: the compiler flag -Dxxx and the CMake variable.
#                       Both indicate the library can be built.
#  - lib1 ... libn:     The extra libraries that need to be linked against the component
#
macro( ORCA_OPTIONAL_SUB_LIBRARY description subdir output_library link_libs ok_to_build definition_tag )

  if( ${${ok_to_build}} )
    message( STATUS "    ${description} - can be built")
    add_subdirectory( ${subdir} )
    
    # The top level executable will be linked to this optional libraries...
    set( sub_link_libraries ${output_library} )
    # ... and all the libraries it depends on.
    foreach( ARG ${ARGN} )
        set( sub_link_libraries ${sub_link_libraries} ${ARG} )
    endforeach( ARG ${ARGN} )
    set( ${link_libs}  ${sub_link_libraries} )
    
    link_directories( ${CMAKE_CURRENT_BINARY_DIR}/${subdir} )
    add_definitions( -D${definition_tag} )
    set(${definition_tag} TRUE)
#    add_library( ${output_library} )
  else(  ${${ok_to_build}} )
    message( STATUS "    ${description} - CANNOT be built")
    set(${definition_tag} FALSE)
  endif( ${${ok_to_build}} )

endmacro( ORCA_OPTIONAL_SUB_LIBRARY description subdir output_library link_libs ok_to_build definition_tag )

