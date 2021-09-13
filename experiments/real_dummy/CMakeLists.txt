cmake_minimum_required(VERSION 3.0)

add_library(real_dummy SHARED 
	factories.cpp
	dummyEA.cpp)
target_include_directories(real_dummy PUBLIC 
	"../../EAFramework/include"
	"../../physicalER/include")
target_link_libraries(real_dummy ARE)

install(TARGETS  real_dummy DESTINATION lib)
install(DIRECTORY . DESTINATION include/real_dummy FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
