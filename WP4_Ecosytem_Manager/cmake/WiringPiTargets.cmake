add_library(WiringPi::WiringPi SHARED IMPORTED)
set_target_properties(WiringPi::WiringPi PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "/usr/include"
        INTERFACE_LINK_LIBRARIES "wiringPi"
        )
