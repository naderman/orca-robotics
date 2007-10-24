#
# Sets up include and link dirs for ZeroC's "Ice" distribution.
# (it contains much more than just Ice)
#

INCLUDE_DIRECTORIES( 
    ${ICE_HOME}/include
)

# Ice libraries installed in lib64 on 64-bit systems.  
# Is there a way to tell automatically?
LINK_DIRECTORIES(
    ${ICE_HOME}/lib
    ${ICE_HOME}/lib64
)
