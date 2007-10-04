INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIceUtil.cmake )

ADD_EXECUTABLE( buffertest buffertest.cpp )
ADD_TEST( BufferTest buffertest )
MESSAGE ( STATUS "Planning to Build Test      : BufferTest [libOrcaIceUtil]")

ADD_EXECUTABLE( proxytest proxytest.cpp )
ADD_TEST( ProxyTest proxytest )
MESSAGE ( STATUS "Planning to Build Test      : ProxyTest [libOrcaIceUtil]")

ADD_EXECUTABLE( storetest storetest.cpp )
ADD_TEST( StoreTest storetest )
MESSAGE ( STATUS "Planning to Build Test      : StoreTest [libOrcaIceUtil]")

ADD_EXECUTABLE( notifytest notifytest.cpp )
ADD_TEST( NotifyTest notifytest )
MESSAGE ( STATUS "Planning to Build Test      : NotifyTest [libOrcaIceUtil]")

ADD_EXECUTABLE( stringtest stringtest.cpp )
ADD_TEST( StringTest stringtest )
MESSAGE ( STATUS "Planning to Build Test      : StringTest [libOrcaIceUtil]")

ADD_EXECUTABLE( mathtest mathtest.cpp )
ADD_TEST( MathTest mathtest )
MESSAGE( STATUS "Planning to Build Test       : MathTest [libOrcaIceUtil]")

ADD_EXECUTABLE( systest systest.cpp )
ADD_TEST( SysTest systest )
MESSAGE ( STATUS "Planning to Build Test      : SysTest [libOrcaIceUtil]")

ADD_EXECUTABLE( threadtest threadtest.cpp )
ADD_TEST( ThreadTest threadtest )
MESSAGE ( STATUS "Planning to Build Test      : ThreadTest [libOrcaIceUtil]")

ADD_EXECUTABLE( safethreadtest safethreadtest.cpp )
ADD_TEST( SafeThreadTest safethreadtest )
MESSAGE ( STATUS "Planning to Build Test      : SafeThreadTest [libOrcaIceUtil]")

ADD_EXECUTABLE( eventqueuetest eventqueuetest.cpp )
ADD_TEST( EventQueueTest eventqueuetest )
MESSAGE ( STATUS "Planning to Build Test      : EventQueueTest [libOrcaIceUtil]")

ADD_EXECUTABLE( jobqueuetest jobqueuetest.cpp )
ADD_TEST( JobQueueTest jobqueuetest )
MESSAGE ( STATUS "Planning to Build Test      : JobQueueTest [libOrcaIceUtil]")
