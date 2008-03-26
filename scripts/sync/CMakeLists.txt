
FILE( GLOB SOURCES *.sh )
INSTALL( PROGRAMS ${SOURCES} DESTINATION scripts )
MESSAGE( STATUS "Will install shell scripts into ${PROJECT_INSTALL_DIR}/scripts" )
