cmake_minimum_required(VERSION 3.0)

find_package(MPI REQUIRED)

set (SOURCES
    mpi_are.c
)

add_executable(mpi_er ${SOURCES})
target_include_directories(mpi_er PUBLIC ${MPI_INCLUDE_PATH})
target_link_libraries(mpi_er ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(mpi_er PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(mpi_er PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
