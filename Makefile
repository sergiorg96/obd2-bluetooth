CXX = g++
CXXFLAGS = -std=c++11 -Wall -o 
CXXFLAGS_DEBUG = -g -std=c++11 -Wall -DDEBUG -o 
ParametroBluetooth = -lbluetooth -lpthread
HPP = src/Obd.hpp src/Commands.hpp src/decoders.hpp src/picangps.hpp src/alarmfile.hpp src/debug.hpp
CPP = src/main.cpp src/decoders.cpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp

CPP_TEST = test/UnitTestCase.cpp src/decoders.cpp
#HPP_TEST = src/decoders.hpp test/Obd-test.hpp
HPP_TEST = src/decoders.hpp src/Obd.hpp src/debug.hpp

main: $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) main $(CPP) $(HPP) $(ParametroBluetooth) 

debug: $(CPP) $(HPP) test/MockSocket
	$(CXX) $(CXXFLAGS_DEBUG) main-debug $(CPP) $(HPP) $(ParametroBluetooth) 

test: $(CPP_TEST) $(HPP_TEST) test/MockSocket.cpp
	$(CXX) $(CXXFLAGS_DEBUG) test/test $(CPP_TEST) $(HPP_TEST) $(ParametroBluetooth)