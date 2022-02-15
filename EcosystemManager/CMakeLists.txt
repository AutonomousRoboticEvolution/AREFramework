cmake_minimum_required(VERSION 3.0)

FILE(GLOB SOURCES src/*)

add_library(ecosystemManager SHARED ${SOURCES})
target_link_libraries(ecosystemManager ARE)
target_include_directories(ecosystemManager PUBLIC
    "include/"
    "../EAFramework/include/")
