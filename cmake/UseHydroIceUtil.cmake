include( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )
include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )

link_libraries( GbxIceUtilAcfr HydroIceUtil )
