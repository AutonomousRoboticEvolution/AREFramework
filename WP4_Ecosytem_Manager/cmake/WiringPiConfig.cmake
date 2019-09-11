get_filename_component(WiringPi_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
#include(CMakeFindDependencyMacro)

#find_dependency(Boost 1.55 REQUIRED COMPONENTS regex)
#find_dependency(RapidJSON 1.0 REQUIRED MODULE)

if(NOT TARGET WiringPi::WiringPi)
    include("${WiringPi_CMAKE_DIR}/WiringPiTargets.cmake")
endif()
