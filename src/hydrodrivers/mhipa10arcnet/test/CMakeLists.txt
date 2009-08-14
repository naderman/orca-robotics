# Test for correct linking.
include_directories( ${${PROJECT_NAME}_SOURCE_DIR}/src 
					 ${${PROJECT_NAME}_SOURCE_DIR}/src/libs )
link_directories( ${${PROJECT_NAME}_SOURCE_DIR}/src/hydrointerfaces )

add_executable( testMHIMessage testMHIMessage.cpp ../unitconverter.cpp ../message.cpp)
target_link_libraries( testMHIMessage HydroInterfaces )

add_executable( testArcnetIO testArcnetIO.cpp ../arcnetio.cpp ../message.cpp ../unitconverter.cpp)
target_link_libraries( testArcnetIO pcap HydroInterfaces )

add_executable( testDriver testDriver.cpp ../arcnetio.cpp ../message.cpp ../response.cpp ../unitconverter.cpp)
target_link_libraries( testDriver pcap HydroInterfaces )        
