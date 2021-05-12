CXX = g++

CFLAGS = -I../../include -I../../remoteApi -DNON_MATLAB_PARSING -DDO_NOT_USE_SHARED_MEMORY -DMAX_EXT_API_CONNECTIONS=255 -Wall -fPIC -static -std=c++1y -Wextra
CFL = -I../../include -I../../remoteApi -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -Wall -fPIC -static

LDFLAGS = -shared

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CFLAGS += -D__Linux
	EXT = sh
else 
	CFLAGS += -D__APPLE__
	EXT = dylib
endif

all:
	@rm -f lib/*.$(EXT)
	@rm -f *.o
		
	gcc $(CFLAGS) -c ../../remoteApi/extApi.c -o extApi.o
	gcc $(CFLAGS) -c ../../remoteApi/extApiPlatform.c -o extApiPlatform.o
#	gcc $(CFLAGS) -c ../../common/shared_memory.c -o shared_memory.o
	
	$(CXX) $(CFLAGS) -c ../../common/scriptFunctionData.cpp -o scriptFunctionData.o
	$(CXX) $(CFLAGS) -c ../../common/scriptFunctionDataItem.cpp -o scriptFunctionDataItem.o
	$(CXX) $(CFLAGS) -c ../../common/v_repLib.cpp -o v_repLib.o
	
	$(CXX) $(CFLAGS) -c ClientEA.cpp -o ClientEA.o
	$(CXX) $(CFLAGS) -c ERClient.cpp -o ERClient.o
		
	$(CXX) $(CFLAGS) -c ../ERFiles/ER.cpp -o ER.o
	$(CXX) $(CFLAGS) -c ../ERFiles/EA.cpp -o EA.o
	$(CXX) $(CFLAGS) -c ../ERFiles/EA_SteadyState.cpp -o EA_SteadyState.o
	$(CXX) $(CFLAGS) -c ../ERFiles/EA_Generational.cpp -o EA_Generational.o
	$(CXX) $(CFLAGS) -c ../ERFiles/Genome.cpp -o Genome.o
	$(CXX) $(CFLAGS) -c ../ERFiles/GenomeFactory.cpp -o GenomeFactory.o
	$(CXX) $(CFLAGS) -c ../ERFiles/Settings.cpp -o Settings.o
	$(CXX) $(CFLAGS) -c ../RandNum.cpp -o RandNum.o	
	$(CXX) $(CFLAGS) -c ../ERFiles/DefaultGenome.cpp -o DefaultGenome.o
	$(CXX) $(CFLAGS) -c ../ERFiles/control/ANN.cpp -o ANN.o
	$(CXX) $(CFLAGS) -c ../ERFiles/control/Control.cpp -o Control.o
	$(CXX) $(CFLAGS) -c ../ERFiles/control/ControlFactory.cpp -o ControlFactory.o
	$(CXX) $(CFLAGS) -c ../ERFiles/control/CPPN.cpp -o CPPN.o
	$(CXX) $(CFLAGS) -c ../ERFiles/control/CustomANN.cpp -o CustomANN.o
	$(CXX) $(CFLAGS) -c ../ERFiles/control/SimpleControl.cpp -o SimpleControl.o
#	$(CXX) $(CFLAGS) -c ../ERFiles/control/TransferNetwork.cpp -o TransferNetwork.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/BaseMorphology.cpp -o BaseMorphology.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/CAT.cpp -o CAT.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/CustomMorphology.cpp -o CustomMorphology.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/EdgarsAmazingMorphologyClass.cpp -o EdgarsAmazingMorphology.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/Development.cpp -o Development.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/ER_CPPN.cpp -o ER_CPPN.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/ER_CPPN_Encoding.cpp -o ER_CPPN_Encoding.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/ER_DirectEncoding.cpp -o ER_DirectEncoding.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/ER_LSystem.cpp -o ER_LSystem.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/FixedBaseMorphology.cpp -o FixedBaseMorphology.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/Morphology.cpp -o Morphology.o
	$(CXX) $(CFLAGS) -c ../ERFiles/morphology/MorphologyFactory.cpp -o MorphologyFactory.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/CPGNeuron.cpp -o CPGNeuron.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/FunctionNeuron.cpp -o FunctionNeuron.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/InputNeuron.cpp -o InputNeuron.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/Neuron.cpp -o Neuron.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/NeuronFactory.cpp -o NeuronFactory.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/PatternNeuron.cpp -o PatternNeuron.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/OutputNeuron.cpp -o OutputNeuron.o
	$(CXX) $(CFLAGS) -c ../ERFiles/neuron/SimpleNeuron.cpp -o SimpleNeuron.o
	
	@mkdir -p lib
	$(CXX) v_repLib.o ER.o Settings.o RandNum.o DefaultGenome.o EA.o EA_SteadyState.o EA_Generational.o Genome.o GenomeFactory.o ANN.o Control.o ControlFactory.o CPPN.o CustomANN.o SimpleControl.o BaseMorphology.o CAT.o EdgarsAmazingMorphology.o Development.o ER_CPPN.o ER_CPPN_Encoding.o ER_DirectEncoding.o CustomMorphology.o ER_LSystem.o FixedBaseMorphology.o Morphology.o MorphologyFactory.o CPGNeuron.o FunctionNeuron.o InputNeuron.o Neuron.o NeuronFactory.o OutputNeuron.o PatternNeuron.o SimpleNeuron.o ClientEA.o ERClient.o extApi.o extApiPlatform.o -o lib/client.$(EXT) -ldl -lpthread