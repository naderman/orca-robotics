#
# Components should add themselves by calling 'GLOBAL_ADD_EXECUTABLE' 
# instead of 'ADD_EXECUTABLE' in CMakeLists.txt.
#
# This gives a centralised location where all components are registered
# and lets us add various things to all components in just one place.
#
# Usage: GLOBAL_ADD_EXECUTABLE( NAME src1 src2 src3 )
#
MACRO( GLOBAL_ADD_EXECUTABLE NAME )
  ADD_EXECUTABLE( ${NAME} ${ARGN} )
  INSTALL_TARGETS( /bin ${NAME} )
  SET ( L ${COMPONENT_LIST} )
  LIST ( APPEND L ${NAME} )
#   MESSAGE ( STATUS "DEBUG: ${L}" )
  SET ( COMPONENT_LIST ${L} CACHE STRING "Global list of components to build" FORCE )
  MESSAGE( STATUS "Planning to Build Executable: ${NAME}" )
ENDMACRO( GLOBAL_ADD_EXECUTABLE NAME )

#
# Components should add themselves by calling 'GLOBAL_ADD_EXECUTABLE' 
# instead of 'ADD_LIBRARY' in CMakeLists.txt.
#
# This gives a centralised location where all components are registered
# and lets us add various things to all components in just one place.
#
# Usage: GLOBAL_ADD_LIBRARY( NAME src1 src2 src3 )
#
MACRO( GLOBAL_ADD_LIBRARY NAME )
  ADD_LIBRARY( ${NAME} ${ARGN} )
  INSTALL_TARGETS( /lib ${NAME} )
  SET ( L ${LIBRARY_LIST} )
  LIST ( APPEND L ${NAME} )
#  MESSAGE ( STATUS "DEBUG: ${L}" )
  SET ( LIBRARY_LIST ${L} CACHE STRING "Global list of libraries to build" FORCE )
  MESSAGE( STATUS "Planning to Build Library   : ${NAME}" )
ENDMACRO( GLOBAL_ADD_LIBRARY NAME )

#
# This is a mechanism to register special items which are not
# components or libraries. This function only records the name of
# the item to display it at the end of the cmake run and to submit
# to the Dashboard.
#
# Usage: GLOBAL_ADD_ITEM( NAME )
#
MACRO( GLOBAL_ADD_ITEM NAME )
  SET ( L ${ITEM_LIST} )
  LIST ( APPEND L ${NAME} )
#  MESSAGE ( STATUS "DEBUG: ${L}" )
  SET ( ITEM_LIST ${L} CACHE STRING "Global list of special items to build" FORCE )
  MESSAGE( STATUS "Planning to Build Item      : ${NAME}" )
ENDMACRO( GLOBAL_ADD_ITEM NAME )
