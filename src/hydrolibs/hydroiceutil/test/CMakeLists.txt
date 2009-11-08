include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseHydroIceUtil.cmake )

add_executable( eventqueuetest eventqueuetest.cpp )
GBX_ADD_TEST( HydroIceUtilEventQueueTest eventqueuetest )

add_executable( eventqueueholdertest eventqueueholdertest.cpp )
GBX_ADD_TEST( HydroIceUtilEventQueueHolderTest eventqueueholdertest )

add_executable( jobqueuetest jobqueuetest.cpp )
GBX_ADD_TEST( HydroIceUtilJobQueueTest jobqueuetest )

add_executable( eventlooptest eventlooptest.cpp )
GBX_ADD_TEST( HydroIceUtilEventLoopTest eventlooptest )

add_executable( eventschedulertest eventschedulertest.cpp )
GBX_ADD_TEST( HydroIceUtilEventSchedulerTest eventschedulertest )

add_executable( localtracertest localtracertest.cpp )
GBX_ADD_TEST( HydroIceUtilLocalTracerTest localtracertest )

add_executable( localstatustest localstatustest.cpp )
GBX_ADD_TEST( HydroIceUtilLocalStatusTest localstatustest )

add_executable( localhistorytest localhistorytest.cpp )
GBX_ADD_TEST( HydroIceUtilLocalHistoryTest localhistorytest )
