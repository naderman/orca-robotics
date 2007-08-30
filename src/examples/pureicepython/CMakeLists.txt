OPTION( BUILD_PURE_ICEPYTHON "Try to build PureIcePython Python Example" ON )

IF ( NOT OS_WIN AND BUILD_PURE_ICEPYTHON )

    PROJECT( pureicepython )

    # TEMP HACKS
    SET ( PROJECT_INTERFACE_PY ${orca_BINARY_DIR}/src/interfaces/python/orca.py )

    # because we are in a separate project we can't refer to the Orca.jar target just by
    # its name, have to give the full path.
#     ADD_CUSTOM_COMMAND(
#         OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pureicepython.py
#         COMMAND ${CMAKE_Python_COMPILER} 
#         ARGS -cp ${CMAKE_CURRENT_BINARY_DIR}:${PROJECT_INTERFACE_PY}:${ICEPYTHON_HOME}/Ice.py
#              -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pureicepython.py
#         MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/pureicepython.py
#         DEPENDS ${PROJECT_INTERFACE_PY}
#         COMMENT "-- Building Python object ${CMAKE_CURRENT_BINARY_DIR}/pureicepython.py" )
# 
#     MESSAGE ( STATUS "DEBUG: ${CMAKE_Python_COMPILER} -cp ${CMAKE_CURRENT_BINARY_DIR} -cp ${PROJECT_INTERFACE_PY} -cp ${ICEPYTHON_HOME}/Ice.py -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/pureicepython.py" )
#     
#     ADD_CUSTOM_TARGET( pureicepython.py ALL
#         DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pureicepython.py)
# 
#     GLOBAL_ADD_ITEM( pureicepython.py )

ENDIF ( NOT OS_WIN AND BUILD_PURE_ICEPYTHON )
