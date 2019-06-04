CXX = g++
CXXFLAGS = -std=c++11 -Wall -o 
CXXFLAGS_DEBUG = -g -std=c++11 -Wall -o 
ParametroBluetooth = -lbluetooth -lpthread
HPP = src/Obd.hpp src/Commands.hpp src/decoders.hpp src/picangps.hpp src/alarmfile.hpp
CPP = src/main.cpp src/decoders.cpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp

CPP_TEST = test/UnitTestCase.cpp

main: $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) main $(CPP) $(HPP) $(ParametroBluetooth) 

debug: $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS_DEBUG) main-debug $(CPP) $(HPP) $(ParametroBluetooth) 

test: $(CPP_TEST)
	$(CXX) $(CXXFLAGS) test/test $(CPP_TEST) 