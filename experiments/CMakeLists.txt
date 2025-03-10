if(VREP_FOUND)
    add_definitions(-DVREP)
elseif(COPPELIASIM_FOUND)
    add_definitions(-DCOPPELIASIM)
endif()

if(WITH_NN2)
    add_subdirectory(nipes)
    add_subdirectory(homeokinesis)
    add_subdirectory(meim)
endif()

install(DIRECTORY common DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
