option( build_PURE_ICEPYTHON "Try to build PureIcePython Python Example" ON )

if( NOT GBX_OS_WIN AND build_PURE_ICEPYTHON )

    PROJECT( pureicepython )

    # TEMP HACKS
    set( PROJECT_INTERFACE_PY ${orca_BINARY_DIR}/src/interfaces/python/orca.py )

    # because we are in a separate project we can't refer to the Orca.jar target just by
    # its name, have to give the full path.
#     add_custom_command(
#         OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pureicepython.py
#         COMMAND ${CMAKE_Python_COMPILER} 
#         ARGS -classpath ${CMAKE_CURRENT_BINARY_DIR}:${PROJECT_INTERFACE_PY}:${ICEPYTHON_HOME}/Ice.py
#              -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pureicepython.py
#         MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/pureicepython.py
#         DEPENDS ${PROJECT_INTERFACE_PY}
#         COMMENT "-- Building Python object ${CMAKE_CURRENT_BINARY_DIR}/pureicepython.py" )
# 
#     message( STATUS "DEBUG: ${CMAKE_Python_COMPILER} -classpath ${CMAKE_CURRENT_BINARY_DIR} -classpath ${PROJECT_INTERFACE_PY} -classpath ${ICEPYTHON_HOME}/Ice.py -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pureicepython.py" )
#     
#     ADD_CUSTOM_TARGET( pureicepython.py ALL
#         DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pureicepython.py)
# 
#     GBX_ADD_ITEM( pureicepython.py )

endif( NOT GBX_OS_WIN AND build_PURE_ICEPYTHON )
