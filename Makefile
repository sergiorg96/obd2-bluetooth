ParametroBluetooth = -lbluetooth -lpthread
HPP = src/Obd.hpp src/Commands.hpp src/decoders.hpp src/picangps.hpp src/alarmfile.hpp
CPP = src/main.cpp src/decoders.cpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp

main: $(CPP) $(HPP)
	g++ -g -std=c++11 -W -o main $(CPP) $(HPP) $(ParametroBluetooth) 