cmake_minimum_required(VERSION 3.0)
project(ARE)

add_compile_options("$<$<CONFIG:RELEASE>:-O3>$<$<CONFIG:DEBUG>:-O0>")
add_compile_options("$<$<CONFIG:DEBUG>:-Wall>")
add_compile_options("$<$<CONFIG:RELEASE>:-DNDEBUG>")
add_compile_options("-rdynamic")


add_subdirectory(modules)
add_subdirectory(EAFramework)
add_subdirectory(simulation)
add_subdirectory(experiments)
