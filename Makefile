CXX = g++
CXXFLAGS = -std=c++17 -Wall -o 
CXXFLAGS_DEBUG = -g -std=c++17 -Wall -DDEBUG -o 
CXXFLAGS_TEST = -g -std=c++17 -Wall -DTEST -DDEBUG -o 
ParametroBluetooth = -lbluetooth -lpthread
HPP = src/Obd.hpp src/Commands.hpp src/decoders.hpp src/picangps.hpp src/alarmfile.hpp src/debug.hpp
CPP = src/main.cpp src/decoders.cpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp
DEPENDENCIAS = test/MockSocket.cpp src/picangps.hpp src/alarmfile.hpp src/Commands.hpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp


CPP_TEST = test/UnitTestCase.cpp src/decoders.cpp
HPP_TEST = src/decoders.hpp src/Obd.hpp src/debug.hpp

main: $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) main $(CPP) $(HPP) $(ParametroBluetooth) 

debug: $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS_DEBUG) main-debug $(CPP) $(HPP) $(ParametroBluetooth) 

test: $(CPP_TEST) $(HPP_TEST) $(DEPENDENCIAS)
	$(CXX) $(CXXFLAGS_TEST) test/test $(CPP_TEST) $(HPP_TEST) $(ParametroBluetooth)

.PHONY: all
all: main debug test