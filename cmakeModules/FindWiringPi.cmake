#find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
#find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)
#
#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(wiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)

# FindWiringPi.cmakeModules
#
# Finds the wiringpi library
#
# This will define the following variables
#
#    WiringPi_FOUND
#    WiringPi_INCLUDE_DIRS
#
# and the following imported targets
#
#     WiringPi::WiringPi
#
# Author: Matteo De Carlo - matteo.dek@gmail.com

find_path(WiringPi_INCLUDE_DIR
        NAMES wiringPi.h
        )

#set(WiringPi_VERSION "2.50")
set(WiringPi_FOUND ON)

mark_as_advanced(WiringPi_FOUND
        WiringPi_INCLUDE_DIR
#        WiringPi_VERSION
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiringPi
        REQUIRED_VARS WiringPi_INCLUDE_DIR
        VERSION_VAR WiringPi_VERSION
        )

if(WiringPi_FOUND)
    set(WiringPi_INCLUDE_DIRS ${WiringPi_INCLUDE_DIR})
endif()

if(WiringPi_FOUND AND NOT TARGET WiringPi::WiringPi)
    add_library(WiringPi::WiringPi INTERFACE IMPORTED)
    set_target_properties(WiringPi::WiringPi PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${WiringPi_INCLUDE_DIR}"
            )
endif()
