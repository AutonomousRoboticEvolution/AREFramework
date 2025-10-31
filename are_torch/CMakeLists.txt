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

add_executable(cpg_test tests/cpg_test.cpp src/torch_nn.cpp)
target_include_directories(cpg_test PUBLIC include ../EAFramework/include)
target_link_libraries(cpg_test "${TORCH_LIBRARIES}")

add_executable(rbf_test tests/rbf_test.cpp src/torch_nn.cpp)
target_include_directories(rbf_test PUBLIC include ../EAFramework/include)
target_link_libraries(rbf_test "${TORCH_LIBRARIES}")

add_executable(cpgrbf_test tests/cpgrbf_test.cpp src/torch_nn.cpp)
target_include_directories(cpgrbf_test PUBLIC include ../EAFramework/include)
target_link_libraries(cpgrbf_test "${TORCH_LIBRARIES}")

add_executable(cpgrbfrnn_test tests/cpgrbfrnn_test.cpp src/torch_nn.cpp)
target_include_directories(cpgrbfrnn_test PUBLIC include ../EAFramework/include)
target_link_libraries(cpgrbfrnn_test "${TORCH_LIBRARIES}")


add_executable(pibb_test tests/pibb_test.cpp src/pi_bb.cpp src/pi_bb_elite.cpp)
target_include_directories(pibb_test PUBLIC include ../EAFramework/include)
target_link_libraries(pibb_test "${TORCH_LIBRARIES}" ARE)

install(TARGETS are_torch DESTINATION lib)
install(DIRECTORY . DESTINATION include/are_torch  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
