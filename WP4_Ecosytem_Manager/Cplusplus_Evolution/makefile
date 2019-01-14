OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__Linux
	EXT = so
else 
	CFLAGS += -D__APPLE__
	EXT = dylib
endif

IDIR="${PWD}/include"
CC=gcc
CXX=g++
CFLAGS=-I${IDIR} -Wall -fPIC -w -std=c++1y
CXXFLAGS=-I${IDIR} -Wall -fPIC -w -std=c++1y
LDFLAGS=-shared

ODIR=obj
LDIR=../lib

LIBS=-lpthread -ldl -shared 

# _DEPS = hellomake.h
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = ERFiles/VREPUI/VREPUI.o \
	ERFiles/DefaultGenomeVREP.o \
	ERFiles/EA_VREP.o \
	ERFiles/ER_VREP.o \
	ERFiles/morphology/MorphologyFactoryVREP.o \
	ERFiles/morphology/ER_DirectEncoding.o \
	ERFiles/morphology/ER_DirectEncodingInterpreter.o \
	ERFiles/morphology/ER_LSystemInterpreter.o \
	ERFiles/morphology/CATVREP.o \
	common/v_repLib.o \
	v_repExtER.o \
	ERFiles/ER.o \
	ERFiles/Settings.o \
	RandNum.o \
	ERFiles/EA.o \
	ERFiles/EA_Factory.o \
	ERFiles/EA_SteadyState.o \
	ERFiles/EA_Generational.o \
	ERFiles/Genome.o \
	ERFiles/GenomeFactory.o \
	ERFiles/GenomeFactoryVREP.o \
	ERFiles/DefaultGenome.o \
	ERFiles/control/ANN.o \
	ERFiles/control/Control.o \
	ERFiles/control/ControlFactory.o \
	ERFiles/control/CPPN.o \
	ERFiles/control/CustomANN.o \
	ERFiles/control/SimpleControl.o \
	ERFiles/env/Environment.o \
	ERFiles/env/EnvironmentFactory.o \
	ERFiles/env/ConstrainedSunEnv.o \
	ERFiles/env/BasicSunEnv.o \
	ERFiles/env/BlockingSunEnv.o \
	ERFiles/env/MoveAndSun.o \
	ERFiles/env/MoveEnv.o \
	ERFiles/env/MovingSunEnv.o \
	ERFiles/env/RoughMoveEnv.o \
	ERFiles/env/WaterEnv.o \
	ERFiles/module/ER_Module.o \
	ERFiles/module/Module_APlantBase.o \
	ERFiles/module/Module_Cube.o \
	ERFiles/module/Module_Leaf.o \
	ERFiles/module/Module_LeafChloe.o \
	ERFiles/module/Module_Servo.o \
	ERFiles/module/Module_Solar.o \
	ERFiles/module/ModuleFactory.o \
	ERFiles/morphology/BaseMorphology.o \
	ERFiles/morphology/CAT.o \
	ERFiles/morphology/Development.o \
	ERFiles/morphology/ER_CPPN.o \
	ERFiles/morphology/ER_CPPN_Encoding.o \
	ERFiles/morphology/ER_CPPN_Interpreter.o \
	ERFiles/morphology/ER_LSystem.o \
	ERFiles/morphology/EdgarsAmazingMorphologyClass.o \
	ERFiles/morphology/EdgarsAmazingMorphologyClassVREP.o \
	ERFiles/morphology/FixedBaseMorphology.o \
	ERFiles/morphology/Morphology.o \
	ERFiles/morphology/CustomMorphology.o \
	ERFiles/morphology/CustomMorphologyVREP.o \
	ERFiles/morphology/MorphologyFactory.o \
	ERFiles/neuron/CPGNeuron.o \
	ERFiles/neuron/FunctionNeuron.o \
	ERFiles/neuron/InputNeuron.o \
	ERFiles/neuron/Neuron.o \
	ERFiles/neuron/NeuronFactory.o \
	ERFiles/neuron/PatternNeuron.o \
	ERFiles/neuron/OutputNeuron.o \
	ERFiles/neuron/SimpleNeuron.o \
	ERFiles/CollisionDetector.o

#	ERFiles/module/Module_Bend.o \
#	ERFiles/module/Module_Brain.o \
#	ERFiles/control/Hormones.o \
#	ERFiles/control/TransferNetwork.o \
#	ERFiles/module/Module_Rectangle.o \
#	ERFiles/module/Module_Spring.o \

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: obj_folders lib/libv_repExtER.$(EXT)

$(ODIR)/%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/%.o: %.cpp $(DEPS) 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

lib/libv_repExtER.$(EXT): $(OBJ)
	@mkdir -p lib
	$(CXX)  $(LDFLAGS) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean obj_folders

obj_folders:
	@mkdir -p obj/common
	@mkdir -p obj/ERFiles/control
	@mkdir -p obj/ERFiles/env
	@mkdir -p obj/ERFiles/module
	@mkdir -p obj/ERFiles/morphology
	@mkdir -p obj/ERFiles/neuron
	@mkdir -p obj/ERFiles/VREPUI

clean:
	# @rm -f lib/*.$(EXT)
	@rm -f *~ core $(INCDIR)/*~ ./*.o
	@find $(ODIR) -name '*.o' -exec rm {} \;
