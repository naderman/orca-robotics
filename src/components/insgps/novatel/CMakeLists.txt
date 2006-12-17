# SET(NOVATEL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
# 
# INCLUDE_DIRECTORIES(
# ${NOVATEL_SRC_DIR}/crc 
# ${NOVATEL_SRC_DIR}/cmd 
# ${NOVATEL_SRC_DIR}/log 
# )
# 
# # SET(CMAKE_INSTALL_PREFIX "/usr/local")
# 
# FILE(GLOB NOVATEL_SOURCES cmd/*.cpp crc/*.cpp log/*.cpp)
# 
# ADD_LIBRARY(novatel SHARED ${NOVATEL_SOURCES} )
# 
# # SET_TARGET_PROPERTIES(insgps PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_MAJOR})
# 
# INSTALL_TARGETS(/lib novatel )
# 
# SUBDIRS(cmd log crc novatel)

#############

SUBDIRS(cmd
        log
        crc)

INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

SET( NOVATEL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

INCLUDE_DIRECTORIES(
${NOVATEL_SRC_DIR}/..
# ${NOVATEL_SRC_DIR}/crc
# ${NOVATEL_SRC_DIR}/cmd
# ${NOVATEL_SRC_DIR}/log
)

FILE(GLOB NOVATEL_SOURCES cmd/*.cpp crc/*.cpp log/*.cpp)

# INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/util )
# INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/sandbox/insgps )

# LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/src/util/serial )

ADD_LIBRARY ( novatel ${NOVATEL_SOURCES} )

# ADD_LIBRARY( OrcaNovatelSpanInsGps SHARED ${SRCS} )

# INSTALL_TARGETS( /lib OrcaNovatelSpanInsGps )
# INSTALL_TARGETS( /lib novatelutil )
# INSTALL_FILES( /include/novatel/ .?\\.h\($|[\ ]\) )


