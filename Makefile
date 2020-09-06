CXX = g++
CXXFLAGS = -std=c++17 -Wall -o
CXXFLAGS_LIB = -std=c++17 -Wall -O -c
CXXFLAGS_LIB_DEBUG = -std=c++17 -Wall $(DEBUG_FLAG) -O -c
CXXFLAGS_LIB_TEST = -std=c++17 -Wall $(DEBUG_FLAG) $(TEST_FLAG) -O -c
CXXFLAGS_LIB_HPP = -std=c++17 -Wall -O -c -x c++
CXXFLAGS_LIB_HPP_DEBUG = -std=c++17 -Wall $(DEBUG_FLAG) -O -c -x c++
CXXFLAGS_LIB_HPP_TEST = -std=c++17 -Wall $(DEBUG_FLAG) $(TEST_FLAG) -O -c -x c++
DEBUG_FLAG = -DDEBUG
TEST_FLAG = -DTEST 
CXXFLAGS_DEBUG = -g -std=c++17 -Wall $(DEBUG_FLAG) -o 
CXXFLAGS_TEST = -g -std=c++17 -Wall $(DEBUG_FLAG) $(TEST_FLAG) -o 
LibreriaBluetooth = -lbluetooth
LibreriaPthread = -lpthread
LibreriaOBD = -lobd2-bluetooth
Librerias = $(LibreriaOBD) $(LibreriaBluetooth) $(LibreriaPthread)
HPP = src/Obd.hpp src/Commands.hpp src/decoders.hpp src/picangps.hpp src/alarmfile.hpp src/debug.hpp
CPP = src/main.cpp src/decoders.cpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp
DEPENDENCIAS = test/MockSocket.cpp src/picangps.hpp src/alarmfile.hpp src/Commands.hpp src/loadcfg.cpp src/picangps.cpp src/alarmfile.cpp


CPP_TEST = test/UnitTestCase.cpp src/decoders.cpp
HPP_TEST = src/decoders.hpp src/Obd.hpp src/debug.hpp

programDTC: main.o libobd2-bluetooth.a
	$(CXX) $(CXXFLAGS) programDTC main.o -Llib $(Librerias)

programDTC-debug: main-debug.o libobd2-bluetooth-debug.a
	$(CXX) $(CXXFLAGS_DEBUG) programDTC-debug main.o -Llib $(Librerias)

test: UnitTestCase.o libobd2-bluetooth-test.a
	$(CXX) $(CXXFLAGS_TEST) test/test UnitTestCase.o -Llib $(Librerias)

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS_LIB) src/main.cpp

main-debug.o: src/main.cpp
	$(CXX) $(CXXFLAGS_LIB_DEBUG) src/main.cpp

UnitTestCase.o: test/UnitTestCase.cpp test/MockSocket.cpp
	$(CXX) $(CXXFLAGS_LIB_TEST) test/UnitTestCase.cpp test/MockSocket.cpp

Obd.o: src/Obd.hpp src/Commands.hpp src/debug.hpp
	$(CXX) $(CXXFLAGS_LIB_HPP) src/Obd.hpp src/Commands.hpp src/debug.hpp

Obd-debug.o: src/Obd.hpp src/Commands.hpp src/debug.hpp
	$(CXX) $(CXXFLAGS_LIB_HPP_DEBUG) src/Obd.hpp src/Commands.hpp src/debug.hpp

Obd-test.o: src/Obd.hpp src/Commands.hpp src/debug.hpp
	$(CXX) $(CXXFLAGS_LIB_HPP_TEST) src/Obd.hpp

decoders.o: src/decoders.cpp src/decoders.hpp
	$(CXX) $(CXXFLAGS_LIB) src/decoders.cpp src/decoders.hpp

alarmfile.o: src/alarmfile.cpp src/alarmfile.hpp
	$(CXX) $(CXXFLAGS_LIB) src/alarmfile.cpp src/alarmfile.hpp

loadcfg.o: src/loadcfg.cpp src/loadcfg.hpp
	$(CXX) $(CXXFLAGS_LIB) src/loadcfg.cpp src/loadcfg.hpp

picangps.o: src/picangps.cpp src/picangps.hpp
	$(CXX) $(CXXFLAGS_LIB) src/picangps.cpp src/picangps.hpp

libobd2-bluetooth.a: Obd.o decoders.o alarmfile.o loadcfg.o picangps.o
	ar rcs lib/libobd2-bluetooth.a Obd.o decoders.o alarmfile.o loadcfg.o picangps.o

libobd2-bluetooth-debug.a: Obd-debug.o decoders.o alarmfile.o loadcfg.o picangps.o
	ar rcs lib/libobd2-bluetooth.a Obd.o decoders.o alarmfile.o loadcfg.o picangps.o

libobd2-bluetooth-test.a: Obd-test.o decoders.o alarmfile.o loadcfg.o picangps.o
	ar rcs lib/libobd2-bluetooth.a Obd.o decoders.o alarmfile.o loadcfg.o picangps.o

##OLD
#main: $(CPP) $(HPP)
#	$(CXX) $(CXXFLAGS) main $(CPP) $(HPP) $(LibreriaBluetooth) $(LibreriaPthread) 

#debug: $(CPP) $(HPP)
#	$(CXX) $(CXXFLAGS_DEBUG) main-debug $(CPP) $(HPP) $(LibreriaBluetooth) $(LibreriaPthread) 

#test: $(CPP_TEST) $(HPP_TEST) $(DEPENDENCIAS)
#	$(CXX) $(CXXFLAGS_TEST) test/test $(CPP_TEST) $(HPP_TEST) $(LibreriaBluetooth) $(LibreriaPthread)

clean:
	rm -f *.o src/*.gch

.PHONY: all
all: programDTC programDTC-debug test clean
