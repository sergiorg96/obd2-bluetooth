ParametroBluetooth = -lbluetooth
HPP = src/Obd.hpp src/Commands.hpp
CPP = src/main.cpp

main: $(CPP) $(HPP)
	g++ -std=c++11 -W -o main $(CPP) $(HPP) $(ParametroBluetooth) 