file( GLOB srcs *.cpp )
set( lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LIBRARY( OrcaProbeTermIostream DEFAULT ${lib_version} ${srcs} )
