#include "Obd.hpp"

typedef  std::map <std::string, std::string> cfgType;

int main(int argc, char **argv)
{
	if (argc != 1){
		printf("Uso: ./main DEVICE-NAME DATO\n");

		return 0;
	}

	cfgType variablesCfg;
	loadCfg("./configuration.cfg", &variablesCfg);
	//std::cout << variablesCfg["BT-DISP-NAME"] << std::endl;

	Obd connection = Obd(variablesCfg["BT-DISP-NAME"].c_str());

	if(connection.isValid()){
		std::cout << "Se ha conectado correctamente" << std::endl;

		//connection.send(connection.map_commands.find(argv[2])->second);
		connection.send(connection.map_commands.find("PIDS_A")->second);
		connection.send(connection.map_commands.find("PIDS_B")->second);
		connection.send(connection.map_commands.find("PIDS_C")->second);
		connection.send(connection.map_commands.find("GET_VIN")->second);
		connection.send(connection.map_commands.find("STATUS")->second);

		connection.printStatus();

		//connection.printPIDs();
		if(connection.getVIN().empty()){
			std::cout << "Vehicle Identification Number no disponible" << std::endl;
		} else {
			std::cout << "Vehicle Identification Number = " << connection.getVIN() << std::endl;
		}

		connection.disconnectBluetooth();
	} else {
		std::cout << "No se ha conectado correctamente" << std::endl;
	}

	return 0;
}