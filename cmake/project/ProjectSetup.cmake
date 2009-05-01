message( STATUS "== PROJECT-SPECIFIC SETUP ==" )
#
# Add project-specific setup
#

# During the install we want to 'label' the install. The SYSTEM_HYDRO_INSTALL
# Variable is needed by the install script as this will most likely be run from
# Root and not have the compelete environment. For some reason I do not understand
# The 'SET - CACHED' variables are not available in the install scripts (djlm)
# install(CODE "set(SYSTEM_ORCA_SOURCE ${PROJECT_SOURCE_DIR})")
# install(CODE "set(SYSTEM_HYDRO_INSTALL ${HYDRO_HOME})")
# install(SCRIPT ${PROJECT_SOURCE_DIR}/cmake/internal/labelInstall.cmake)

