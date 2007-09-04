# annoying: including these rules causes test .cfg files to be installed.
INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( ptrbuffertest ptrbuffertest.cpp )
ADD_TEST( PtrBufferTest ptrbuffertest )
MESSAGE ( STATUS "Planning to Build Test      : PtrBufferTest [libOrcaIce]")

ADD_EXECUTABLE( ptrproxytest ptrproxytest.cpp )
ADD_TEST( PtrProxyTest ptrproxytest )
MESSAGE ( STATUS "Planning to Build Test      : PtrProxyTest [libOrcaIce]")

ADD_EXECUTABLE( ptrnotifytest ptrnotifytest.cpp )
ADD_TEST( PtrNotifyTest ptrnotifytest )
MESSAGE ( STATUS "Planning to Build Test      : PtrNotifyTest [libOrcaIce]")

ADD_EXECUTABLE( buffertest buffertest.cpp )
ADD_TEST( BufferTest buffertest )
MESSAGE ( STATUS "Planning to Build Test      : BufferTest [libOrcaIce]")

ADD_EXECUTABLE( proxytest proxytest.cpp )
ADD_TEST( ProxyTest proxytest )
MESSAGE ( STATUS "Planning to Build Test      : ProxyTest [libOrcaIce]")

ADD_EXECUTABLE( notifytest notifytest.cpp )
ADD_TEST( NotifyTest notifytest )
MESSAGE ( STATUS "Planning to Build Test      : NotifyTest [libOrcaIce]")

ADD_EXECUTABLE( stringtest stringtest.cpp )
ADD_TEST( StringTest stringtest )
MESSAGE ( STATUS "Planning to Build Test      : StringTest [libOrcaIce]")

ADD_EXECUTABLE( systest systest.cpp )
ADD_TEST( SysTest systest )
MESSAGE ( STATUS "Planning to Build Test      : SysTest [libOrcaIce]")

ADD_EXECUTABLE( threadtest threadtest.cpp )
ADD_TEST( ThreadTest threadtest )
MESSAGE ( STATUS "Planning to Build Test      : ThreadTest [libOrcaIce]")

ADD_EXECUTABLE( safethreadtest safethreadtest.cpp )
ADD_TEST( SafeThreadTest safethreadtest )
MESSAGE ( STATUS "Planning to Build Test      : SafeThreadTest [libOrcaIce]")

ADD_EXECUTABLE( eventqueuetest eventqueuetest.cpp )
ADD_TEST( EventQueueTest eventqueuetest )
MESSAGE ( STATUS "Planning to Build Test      : EventQueueTest [libOrcaIce]")

ADD_EXECUTABLE( proptest proptest.cpp )
ADD_TEST( PropUtilsTest proptest )
MESSAGE ( STATUS "Planning to Build Test      : PropUtilsTest [libOrcaIce]")

ADD_EXECUTABLE( privateutilstest privateutilstest.cpp )
ADD_TEST( PrivateUtilsTest privateutilstest )
MESSAGE ( STATUS "Planning to Build Test      : PrivateUtilsTest [libOrcaIce]")

ADD_EXECUTABLE( propfiletest propfiletest.cpp )
ADD_TEST( PropFileTest propfiletest )
MESSAGE ( STATUS "Planning to Build Test      : PropFileTest [libOrcaIce]")

# basic app test
ADD_EXECUTABLE( apptest apptest.cpp )
ADD_TEST( AppTest apptest )
GENERATE_CONFIG_FILE( apptest.def apptest 0 )
MESSAGE ( STATUS "Planning to Build Test      : AppTest [libOrcaIce]")

# advanced app tests
ADD_EXECUTABLE( appproptest appproptest.cpp )
# The standard CFG_INTDIR does not seem to get resolved here, do it manually
IF( OS_WIN )
    # VCC defaults to Debug
    SET( MANUAL_CFG_INTDIR "debug" )
ELSE( OS_WIN )
    SET( MANUAL_CFG_INTDIR "." )
ENDIF( OS_WIN )
# settings: FACT=0, GLOBAL=200, LOCAL=300, ARGS=400
# test1000 = factory YES, global NO, local NO, args NO --> expect=FACT, which is 0
# NOTE: the CTEST variable is escaped here so it's ignored by CMake and is put into the CTest script which then
# expands it to debug/release depending on the -C parameter (Debug or Release).
ADD_TEST( AppPropTest1000 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --AppPropTest.Expect=0 )
ADD_TEST( AppPropTest1001 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Expect=400 )
ADD_TEST( AppPropTest1010 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --AppPropTest.Expect=300 )
ADD_TEST( AppPropTest1011 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Expect=400 )
ADD_TEST( AppPropTest1100 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --AppPropTest.Expect=200 )
ADD_TEST( AppPropTest1101 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Expect=400 )
ADD_TEST( AppPropTest1110 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --AppPropTest.Expect=300 )
ADD_TEST( AppPropTest1111 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/apppropteston.cfg --Orca.Tracer.ErrorToFile=400 --AppPropTest.Expect=400 )
# now try to fail it
# it's ok to run a component without global, local, or both config files
ADD_TEST( AppPropTest1x00 appproptest --Orca.GlobalConfig=crap.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/appproptestoff.cfg --AppPropTest.Expect=0 )
# without a comp config file, have to specify Orca.RequireRegistry on command line
ADD_TEST( AppPropTest1xx0 appproptest --Orca.GlobalConfig=crap.cfg --Orca.Config=./\${CTEST_CONFIGURATION_TYPE}/crap.cfg --AppPropTest.Expect=0 --Orca.RequireRegistry=0 )
ADD_TEST( AppPropTest11x0 appproptest --Orca.GlobalConfig=${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg --AppPropTest.Expect=200 --Orca.RequireRegistry=0 )
GENERATE_CONFIG_FILE( apppropteston.def appproptest 0 )
GENERATE_CONFIG_FILE( appproptestoff.def appproptest 0 )
MESSAGE ( STATUS "Planning to Build 10 Tests  : AppPropTestXXXX [libOrcaIce]")

# can't do this in win 'cause: 1) need shared libs for services, 2) need to copy files

IF ( NOT OS_WIN )

# basic service test
ADD_LIBRARY( ServiceTest SHARED servicetest )
ADD_TEST( ServiceTest ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/iceboxbasic.cfg )
MESSAGE ( STATUS "Planning to Build Test      : ServiceTest [libOrcaIce]")

# advanced service tests
ADD_LIBRARY( ServicePropTest SHARED serviceproptest )
# settings: FACT=0, GLOBAL=200, LOCAL=300, ARGS=400
# test1000 = factory YES, global NO, local NO, args NO --> expect=FACT=0
# NOTE: have to copy these files because, unlike with the application, here we CANNOT specify the location
# of the global file on command line (command line args for individual services are hardwired 
# into the icebox.cfg file)
ADD_CUSTOM_COMMAND( TARGET ServicePropTest POST_BUILD COMMAND ${CMAKE_COMMAND} 
                    ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/globalon.cfg ${CMAKE_CURRENT_BINARY_DIR}/. )
ADD_CUSTOM_COMMAND( TARGET ServicePropTest POST_BUILD COMMAND ${CMAKE_COMMAND} 
                    ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/globaloff.cfg ${CMAKE_CURRENT_BINARY_DIR}/. )
GENERATE_CONFIG_FILE( serviceproptest2on.def ServicePropTest 0 )
GENERATE_CONFIG_FILE( serviceproptest2off.def ServicePropTest 0 )
GENERATE_CONFIG_FILE( serviceproptest3on.def ServicePropTest 0 )
GENERATE_CONFIG_FILE( serviceproptest3off.def ServicePropTest 0 )
# each of these icebox config files defines 3 services:
#   a) service with properties in the same file
#   b) service with properties in a separate file sharing Communicator with service (a)
#   c) service with properties in a separate file having it's own Communicator
ADD_TEST( ServicePropTest1000 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1000.cfg )
ADD_TEST( ServicePropTest1001 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1001.cfg )
ADD_TEST( ServicePropTest1010 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1010.cfg )
ADD_TEST( ServicePropTest1011 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1011.cfg )
ADD_TEST( ServicePropTest1100 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1100.cfg )
ADD_TEST( ServicePropTest1101 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1101.cfg )
ADD_TEST( ServicePropTest1110 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1110.cfg )
ADD_TEST( ServicePropTest1111 ${ICE_HOME}/bin/icebox --Ice.Config=${CMAKE_CURRENT_SOURCE_DIR}/icebox1111.cfg )
MESSAGE ( STATUS "Planning to Build 8  Tests  : ServicePropTestXXXX [libOrcaIce]")
ENDIF ( NOT OS_WIN )

ADD_EXECUTABLE( configtest configtest.cpp )
ADD_TEST( ConfigUtilsTest configtest )
GENERATE_CONFIG_FILE( configtest.def configtest 0 )
MESSAGE ( STATUS "Planning to Build Test      : ConfigUtilsTest [libOrcaIce]")

# alexm: 
# This test relies on establishing direct connections which is currently broken
# not sure why it works in linux actually.
IF ( NOT OS_QNX )
ADD_EXECUTABLE( connecttest connecttest.cpp )
ADD_TEST( ConnectUtilsTest connecttest )
GENERATE_CONFIG_FILE( connecttest.def connecttest 0 )
MESSAGE ( STATUS "Planning to Build Test      : ConnectUtilsTest [libOrcaIce]")
ENDIF ( NOT OS_QNX )
