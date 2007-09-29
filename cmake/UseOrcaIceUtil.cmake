INCLUDE_DIRECTORIES( 
    ${ICE_HOME}/include
)

# Ice libraries installed in lib64 on 64-bit systems.  Is there a way to tell automatically?
LINK_DIRECTORIES(
    ${ICE_HOME}/lib
    ${ICE_HOME}/lib64
)

INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/libs
)

LINK_LIBRARIES( OrcaIceUtil IceUtil )
