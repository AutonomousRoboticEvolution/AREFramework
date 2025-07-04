cmake_minimum_required(VERSION 3.0)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
file(GLOB SOURCES "src/*")
add_library(are_torch SHARED ${SOURCES})
target_include_directories(are_torch PUBLIC 
    include
    ../EAFramework/include
)
target_link_libraries(are_torch ARE simulatedER "${TORCH_LIBRARIES}")

install(TARGETS are_torch DESTINATION lib/)
install(DIRECTORY . DESTINATION include/are_torch  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
