ParametroBluetooth = -lbluetooth
HPP = Obd.hpp Commands.hpp

main: main.cpp $(HPP)
	g++ -std=c++11 -W -o main main.cpp $(HPP) $(ParametroBluetooth) 