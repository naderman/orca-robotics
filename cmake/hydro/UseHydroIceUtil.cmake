include( ${HYDRO_CMAKE_DIR}/UseHydroUtil.cmake )
include( ${HYDRO_CMAKE_DIR}/UseIceUtil.cmake )

link_libraries( GbxIceUtilAcfr HydroIceUtil )
