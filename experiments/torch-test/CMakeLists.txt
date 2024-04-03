cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(example-app)

#set(LIBTORCH_FOLDER "/home/leni/Downloads/libtorch/")
set(c10_LIBRARY "${LIBTORCH_FOLDER}/lib/")
set(kineto_LIBRARY "${LIBTORCH_FOLDER}/lib/")
set(Torch_DIR "${LIBTORCH_FOLDER}/share/cmake/Torch/")
set(Caffe2_DIR "${LIBTORCH_FOLDER}/share/cmake/Caffe2/")

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")





add_executable(example-app example-app.cpp)
target_link_libraries(example-app "${TORCH_LIBRARIES}")
set_property(TARGET example-app PROPERTY CXX_STANDARD 17)

add_executable(ff_nn ff_nn.cpp)
target_link_libraries(ff_nn "${TORCH_LIBRARIES}")
set_property(TARGET ff_nn PROPERTY CXX_STANDARD 17)
