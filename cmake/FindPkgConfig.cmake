## FindPkgConfig.cmake
## by  Albert Strasheim <http://students . ee . sun . ac . za/~albert/>
## and Alex Brooks (a.brooks at acfr . usyd . edu . au)
##
## This module finds packages using pkg-config, which retrieves
## information about packages from special metadata files.
##
## See http://www . freedesktop . org/Software/pkgconfig/
##
## -------------------------------------------------------------------
##
## Usage:
##
## include( ${CMAKE_ROOT}/Modules/FindPkgConfig.cmake)
## 
## if( CMAKE_PKGCONFIG_EXECUTABLE )
##
##     # Find all the librtk stuff with pkg-config
##     PKGCONFIG( "librtk >= 2.0" HAVE_RTK RTK_INCLUDE_DIRS RTK_DEFINES RTK_LINK_DIRS RTK_LIBS )
##
## else( CMAKE_PKGCONFIG_EXECUTABLE )
##
##     # Can't find pkg-config -- have to find librtk somehow else
##
## endif( CMAKE_PKGCONFIG_EXECUTABLE )
##
##
## Notes:
## 
## You can set the PKG_CONFIG_PATH environment variable to tell
## pkg-config where to search for .pc files. See pkg-config(1) for
## more information.
##
#
# FIXME: if(WIN32) pkg-config --msvc-syntax endif(WIN32) ???
#
# FIXME: Parsing of pkg-config output is specific to gnu-style flags
#

FIND_PROGRAM(CMAKE_PKGCONFIG_EXECUTABLE pkg-config)
mark_as_advanced(CMAKE_PKGCONFIG_EXECUTABLE)

########################################

macro(PKGCONFIG_PARSE_FLAGS FLAGS INCLUDES DEFINES)

  #message("DEBUG: FLAGS: ${FLAGS}")

  string(REGEX MATCHALL "-I[^ ]*" ${INCLUDES} "${FLAGS}")
  string(REGEX REPLACE "-I" "" ${INCLUDES} "${${INCLUDES}}")
  #message("DEBUG: INCLUDES: ${${INCLUDES}}")
  
  string(REGEX REPLACE "-I[^ ]*" "" ${DEFINES} "${FLAGS}")
  #message("DEBUG: DEFINES: ${${DEFINES}}")

endmacro(PKGCONFIG_PARSE_FLAGS)

########################################

macro(PKGCONFIG_PARSE_LIBS LIBS LINKDIRS LINKLIBS)

  #message("DEBUG: LIBS: ${LIBS}")

  string(REGEX MATCHALL "-L[^ ]*" ${LINKDIRS} "${LIBS}")
  string(REGEX REPLACE "-L" "" ${LINKDIRS} "${${LINKDIRS}}")
  #message("DEBUG: LINKDIRS: ${${LINKDIRS}}")

  string(REGEX MATCHALL "-l[^ ]*" ${LINKLIBS} "${LIBS}")
  string(REGEX REPLACE "-l" "" ${LINKLIBS} "${${LINKLIBS}}")
  #message("DEBUG: LINKLIBS: ${${LINKLIBS}}")

endmacro(PKGCONFIG_PARSE_LIBS)

########################################

macro(PKGCONFIG LIBRARY FOUND INCLUDE_DIRS DEFINES LINKDIRS LINKLIBS)

  set(${FOUND} 0)

# alexm: why print it twice? once here, and once when it's found/not found
#   message("-- Looking for ${LIBRARY}")
  
  if(CMAKE_PKGCONFIG_EXECUTABLE)
    # message("DEBUG: pkg-config executable found")
    
    exec_program(${CMAKE_PKGCONFIG_EXECUTABLE}
      ARGS "'${LIBRARY}'"
      OUTPUT_VARIABLE PKGCONFIG_OUTPUT
      RETURN_VALUE PKGCONFIG_RETURN)

    if(NOT PKGCONFIG_RETURN)
      
      # set C_FLAGS and CXX_FLAGS
      exec_program(${CMAKE_PKGCONFIG_EXECUTABLE}
        ARGS "--cflags '${LIBRARY}'"
        OUTPUT_VARIABLE CMAKE_PKGCONFIG_C_FLAGS)

      #set(CMAKE_PKGCONFIG_CXX_FLAGS "${CMAKE_PKGCONFIG_C_FLAGS}")
      PKGCONFIG_PARSE_FLAGS( "${CMAKE_PKGCONFIG_C_FLAGS}" ${INCLUDE_DIRS} ${DEFINES} )
      
      # set LIBRARIES
      exec_program(${CMAKE_PKGCONFIG_EXECUTABLE}
        ARGS "--libs '${LIBRARY}'"
        OUTPUT_VARIABLE CMAKE_PKGCONFIG_LIBRARIES)
      PKGCONFIG_PARSE_LIBS ( "${CMAKE_PKGCONFIG_LIBRARIES}" ${LINKDIRS} ${LINKLIBS} )

      set(${FOUND} 1)
      message( STATUS "Looking for ${LIBRARY} using pkgconfig -- found")

    else(NOT PKGCONFIG_RETURN)
      message( STATUS "Looking for ${LIBRARY} using pkgconfig -- not found")
      
      set(CMAKE_PKGCONFIG_C_FLAGS "")
      set(CMAKE_PKGCONFIG_CXX_FLAGS "")
      set(CMAKE_PKGCONFIG_LIBRARIES "")
      set(${INCLUDE_DIRS} "")
      set(${DEFINES} "")
      set(${LINKDIRS} "")
      set(${LINKLIBS} "")

    endif(NOT PKGCONFIG_RETURN)

  else(CMAKE_PKGCONFIG_EXECUTABLE)
    message( STATUS "pkg-config executable NOT FOUND")
  endif(CMAKE_PKGCONFIG_EXECUTABLE)

  #message("Have  ${LIBRARY}       : ${${FOUND}}")
  #message("${LIBRARY} include dirs: ${${INCLUDE_DIRS}}")
  #message("${LIBRARY} defines     : ${${DEFINES}}")
  #message("${LIBRARY} link dirs   : ${${LINKDIRS}}")
  #message("${LIBRARY} link libs   : ${${LINKLIBS}}")

endmacro(PKGCONFIG)