cmake_minimum_required(VERSION 3.0)
project(ER)

set(SOURCES
    ERFiles/CollisionDetector.cpp
    ERFiles/control/ANN.cpp
    ERFiles/control/Control.cpp
    ERFiles/control/ControlFactory.cpp
    ERFiles/control/CPPN.cpp
    ERFiles/control/CustomANN.cpp
    ERFiles/control/SimpleControl.cpp
    ERFiles/DefaultGenome.cpp
    ERFiles/DefaultGenomeVREP.cpp
    ERFiles/EA_Factory.cpp
    ERFiles/EA_Generational.cpp
    ERFiles/EA_SteadyState.cpp
    ERFiles/EA_VREP.cpp
    ERFiles/EA.cpp
    ERFiles/env/BasicSunEnv.cpp
    ERFiles/env/BlockingSunEnv.cpp
    ERFiles/env/ConstrainedSunEnv.cpp
    ERFiles/env/Environment.cpp
    ERFiles/env/EnvironmentFactory.cpp
    ERFiles/env/MoveAndSun.cpp
    ERFiles/env/MoveEnv.cpp
    ERFiles/env/MovingSunEnv.cpp
    ERFiles/env/RoughMoveEnv.cpp
    ERFiles/env/WaterEnv.cpp
    ERFiles/ER_VREP.cpp
    ERFiles/ER.cpp
    ERFiles/Genome.cpp
    ERFiles/GenomeFactory.cpp
    ERFiles/GenomeFactoryVREP.cpp
    ERFiles/module/ER_Module.cpp
    ERFiles/module/Module_APlantBase.cpp
    ERFiles/module/Module_Cube.cpp
    ERFiles/module/Module_Leaf.cpp
    ERFiles/module/Module_LeafChloe.cpp
    ERFiles/module/Module_Servo.cpp
    ERFiles/module/Module_Solar.cpp
    ERFiles/module/ModuleFactory.cpp
    ERFiles/morphology/BaseMorphology.cpp
    ERFiles/morphology/CAT.cpp
    ERFiles/morphology/CATVREP.cpp
    ERFiles/morphology/CustomMorphology.cpp
    ERFiles/morphology/CustomMorphologyVREP.cpp
    ERFiles/morphology/Development.cpp
    ERFiles/morphology/EdgarsAmazingMorphologyClass.cpp
    ERFiles/morphology/EdgarsAmazingMorphologyClassVREP.cpp
    ERFiles/morphology/ER_CPPN_Encoding.cpp
    ERFiles/morphology/ER_CPPN_Interpreter.cpp
    ERFiles/morphology/ER_CPPN.cpp
    ERFiles/morphology/ER_DirectEncoding.cpp
    ERFiles/morphology/ER_DirectEncodingInterpreter.cpp
    ERFiles/morphology/ER_LSystem.cpp
    ERFiles/morphology/ER_LSystemInterpreter.cpp
    ERFiles/morphology/FixedBaseMorphology.cpp
    ERFiles/morphology/Morphology.cpp
    ERFiles/morphology/MorphologyFactory.cpp
    ERFiles/morphology/MorphologyFactoryVREP.cpp
    ERFiles/neuron/CPGNeuron.cpp
    ERFiles/neuron/FunctionNeuron.cpp
    ERFiles/neuron/InputNeuron.cpp
    ERFiles/neuron/Neuron.cpp
    ERFiles/neuron/NeuronFactory.cpp
    ERFiles/neuron/OutputNeuron.cpp
    ERFiles/neuron/PatternNeuron.cpp
    ERFiles/neuron/SimpleNeuron.cpp
    ERFiles/Settings.cpp
    ERFiles/VREPUI/VREPUI.cpp
    RandNum.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/vrep-programming/common/v_repLib.cpp"
    #	ERFiles/control/Hormones.cpp
    #	ERFiles/control/TransferNetwork.cpp
    #	ERFiles/module/Module_Bend.cpp
    #	ERFiles/module/Module_Brain.cpp
    #	ERFiles/module/Module_Rectangle.cpp
    #	ERFiles/module/Module_Spring.cpp
)

add_library(ER_library STATIC ${SOURCES})


target_include_directories(ER_library PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vrep-programming/include"
)

set_target_properties(ER_library PROPERTIES 
    CXX_STANDARD 14
    # COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

add_library(v_repExtER SHARED 
    v_repExtER.cpp
)

target_link_libraries(v_repExtER ER_library)

target_include_directories(v_repExtER PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vrep-programming/include"
)

set_target_properties(v_repExtER PROPERTIES 
    CXX_STANDARD 14
    # COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(v_repExtER PUBLIC "__WINDOWS__")
    target_compile_definitions(ER_library PUBLIC "__WINDOWS__")
elseif(APPLE)
    target_compile_definitions(v_repExtER PUBLIC "__APPLE__")
    target_compile_definitions(ER_library PUBLIC "__APPLE__")
elseif(UNIX)    
    target_compile_definitions(v_repExtER PUBLIC "__linux")
    target_compile_definitions(ER_library PUBLIC "__linux")
endif()

add_subdirectory(ERClient)
add_subdirectory(MPI)
