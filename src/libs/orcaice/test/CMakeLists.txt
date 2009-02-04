INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

ADD_EXECUTABLE( ptrbuffertest ptrbuffertest.cpp )
GBX_ADD_TEST( OrcaIce_PtrBuffer ptrbuffertest )

ADD_EXECUTABLE( ptrstoretest ptrstoretest.cpp )
GBX_ADD_TEST( OrcaIce_PtrStore ptrstoretest )

ADD_EXECUTABLE( ptrnotifytest ptrnotifytest.cpp )
GBX_ADD_TEST( OrcaIce_PtrNotify ptrnotifytest )

ADD_EXECUTABLE( proptest proptest.cpp )
GBX_ADD_TEST( OrcaIce_PropUtils proptest )

ADD_EXECUTABLE( timetest timetest.cpp )
GBX_ADD_TEST( OrcaIce_Time timetest )

# admin interfaces test
ADD_EXECUTABLE( admintest admintest.cpp )
GBX_ADD_TEST( OrcaIce_Admin admintest )
ORCA_GENERATE_CONFIG_FILE( admintest.def admintest 0 )

# basic app test, no standard interface, no adapter activation
ADD_EXECUTABLE( apptest apptest.cpp )
GBX_ADD_TEST( OrcaIce_App apptest )

# test std component interfaces
ADD_EXECUTABLE( compstdifacetest compstdifacetest.cpp )
GBX_ADD_TEST( OrcaIce_CompStdIface compstdifacetest )
ORCA_GENERATE_CONFIG_FILE( compstdifacetest.def compstdifacetest 0 )

# testing platform and component name substitutions
ADD_EXECUTABLE( compnametest compnametest.cpp )
# explicit platform & comp spec
GBX_ADD_TEST( OrcaIce_CompName_ExplP_ExplC compnametest --Test.Platform=horseshit --Test.Component=comptest --Test.Config.ExpPlatfName=horseshit --Test.Config.ExpCompName=comptest --Orca.Component.PrintProperties=1 )
# local platform spec
GBX_ADD_TEST( OrcaIce_CompName_LocP_NullC compnametest --Test.Platform=local --Test.Config.ExpCompName=test --Orca.Component.PrintProperties=1 )
# empty platorm spec
GBX_ADD_TEST( OrcaIce_CompName_NullP_NullC compnametest --Test.Config.ExpCompName=test --Orca.Component.PrintProperties=1 )
# specifying AdapterId
GBX_ADD_TEST( OrcaIce_CompName_AdapterId compnametest --Test.AdapterId=horseshit/comptest --Test.Platform=local --Test.Config.ExpPlatfName=horseshit --Test.Config.ExpCompName=comptest --Orca.Component.PrintProperties=1 )

# advanced app tests
ADD_EXECUTABLE( appproptest appproptest.cpp )
# The standard CFG_INTDIR does not seem to get resolved here, do it manually
IF( GBX_OS_WIN )
    # VCC defaults to Debug
    SET( MANUAL_CFG_INTDIR "debug" )
ELSE( GBX_OS_WIN )
    SET( MANUAL_CFG_INTDIR "." )
ENDIF( GBX_OS_WIN )
# settings: FACT=0, GLOBAL=200, LOCAL=300, ARGS=400
# test1000 = factory YES, global NO, local NO, args NO --> expect=FACT, which is 0
# NOTE: the CTEST variable is escaped here so it's ignored by CMake and is put into the C script which then
# expands it to debug/release depending on the -C parameter (Debug or Release).
GBX_ADD_TEST( OrcaIce_AppProp1000 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --AppPropTest.Config.Expect=0 )
GBX_ADD_TEST( OrcaIce_AppProp1001 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Config.Expect=400 )
GBX_ADD_TEST( OrcaIce_AppProp1010 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --AppPropTest.Config.Expect=300 )
GBX_ADD_TEST( OrcaIce_AppProp1011 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Config.Expect=400 )
GBX_ADD_TEST( OrcaIce_AppProp1100 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --AppPropTest.Config.Expect=200 )
GBX_ADD_TEST( OrcaIce_AppProp1101 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Config.Expect=400 )
GBX_ADD_TEST( OrcaIce_AppProp1110 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --AppPropTest.Config.Expect=300 )
GBX_ADD_TEST( OrcaIce_AppProp1111 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Config.Expect=400 )
# now try to fail it
# it's ok to run a component without global, local, or both config files
GBX_ADD_TEST( OrcaIce_AppProp1x00 appproptest --Orca.GlobalConfig=crap.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --AppPropTest.Config.Expect=0 )
# without a comp config file, have to specify Orca.RequireRegistry on command line
GBX_ADD_TEST( OrcaIce_AppProp1xx0 appproptest --Orca.GlobalConfig=crap.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/crap.cfg --AppPropTest.Config.Expect=0 --Orca.RequireRegistry=0 )
GBX_ADD_TEST( OrcaIce_AppProp11x0 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --AppPropTest.Config.Expect=200 --Orca.RequireRegistry=0 )
ORCA_GENERATE_CONFIG_FILE( apppropteston.def appproptest 0 )
ORCA_GENERATE_CONFIG_FILE( appproptestoff.def appproptest 0 )

# can't do this in win 'cause: 1) need shared libs for services, 2) need to copy files

IF( NOT GBX_OS_WIN )

# basic service test
# Do NOT use DEFAULT setting here!
ADD_LIBRARY( ServiceTest SHARED servicetest )
GBX_ADD_TEST( OrcaIce_Service ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/iceboxbasic.cfg )

# advanced service tests
# Do NOT use DEFAULT setting here!
ADD_LIBRARY( ServicePropTest SHARED serviceproptest )
# settings: FACT=0, GLOBAL=200, LOCAL=300, ARGS=400
# test1000 = factory YES, global NO, local NO, args NO --> expect=FACT=0

# NOTE: have to copy these files because, unlike with the application, here we CANNOT specify the location
# of the global file on command line (command line args for individual services are hardwired 
# into the icebox.cfg file)
# alexm: this stopped working for some reason
# ADD_CUSTOM_COMMAND( TARGET ServiceProp POST_BUILD COMMAND ${CMAKE_COMMAND} 
#                     ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg ${CMAKE_CURRENT_BINARY_DIR}/. )
# ADD_CUSTOM_COMMAND( TARGET ServiceProp POST_BUILD COMMAND ${CMAKE_COMMAND} 
#                     ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg ${CMAKE_CURRENT_BINARY_DIR}/. )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg ${CMAKE_CURRENT_BINARY_DIR}/globalon.cfg COPYONLY )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg ${CMAKE_CURRENT_BINARY_DIR}/globaloff.cfg COPYONLY )

ORCA_GENERATE_CONFIG_FILE( serviceproptest2on.def ServicePropTest 0 )
ORCA_GENERATE_CONFIG_FILE( serviceproptest2off.def ServicePropTest 0 )
ORCA_GENERATE_CONFIG_FILE( serviceproptest3on.def ServicePropTest 0 )
ORCA_GENERATE_CONFIG_FILE( serviceproptest3off.def ServicePropTest 0 )
# each of these icebox config files defines 3 services:
#   a) service with properties in the same file
#   b) service with properties in a separate file sharing Communicator with service (a)
#   c) service with properties in a separate file having it's own Communicator
GBX_ADD_TEST( OrcaIce_ServiceProp1000 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1000.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1001 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1001.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1010 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1010.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1011 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1011.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1100 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1100.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1101 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1101.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1110 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1110.cfg )
GBX_ADD_TEST( OrcaIce_ServiceProp1111 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1111.cfg )
ENDIF( NOT GBX_OS_WIN )

ADD_EXECUTABLE( configtest configtest.cpp )
GBX_ADD_TEST( OrcaIce_ConfigUtils configtest )
ORCA_GENERATE_CONFIG_FILE( configtest.def configtest 0 )

ADD_EXECUTABLE( converttest converttest.cpp )
GBX_ADD_TEST( OrcaIce_ConvertUtils converttest )

ADD_EXECUTABLE( icetest icetest.cpp )
GBX_ADD_TEST( OrcaIce_IceUtils icetest )
ORCA_GENERATE_CONFIG_FILE( icetest.def icetest 0 )

# alexm: 
# This test relies on establishing direct connections which is currently broken
# not sure why it works in linux actually.
IF( NOT GBX_OS_QNX )
ADD_EXECUTABLE( connecttest connecttest.cpp )
GBX_ADD_TEST( OrcaIce_ConnectUtils connecttest )
ORCA_GENERATE_CONFIG_FILE( connecttest.def connecttest 0 )
ENDIF( NOT GBX_OS_QNX )

ADD_EXECUTABLE( icegridtest icegridtest.cpp )
GBX_ADD_TEST( OrcaIce_IceGridUtils icegridtest )
ORCA_GENERATE_CONFIG_FILE( icegridtest.def icegridtest 0 )

ADD_EXECUTABLE( substatusthreadtest substatusthreadtest.cpp )
GBX_ADD_TEST( OrcaIce_SubstatusThread substatusthreadtest )

ADD_EXECUTABLE( subsystemthreadtest subsystemthreadtest.cpp )
GBX_ADD_TEST( OrcaIce_SubsystemThread subsystemthreadtest )

ADD_EXECUTABLE( catchutilstest catchutilstest.cpp )
GBX_ADD_TEST( OrcaIce_CatchUtils catchutilstest )
