#include "Obd.hpp"

typedef  std::map <std::string, std::string> cfgType;

int main(int argc, char **argv)
{
	if (argc != 3){
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

		//connection.map_commands.find(argv[2])->second;
		/*
		std::cout << "Lista de comandos disponibles:" << std::endl << std::endl;
		std::cout << "\tComando\t\tBytes" << std::endl;
		for (int i = 0; i < (int)connection.vec_commands.size(); ++i)
		{
			std::cout << "\t" << connection.vec_commands[i].getName() << "\t\t" << connection.vec_commands[i].getCMD() << std::endl;	
		}
		*/
		//std::cout << "Comando a ejecutar: " << connection.map_commands.find("RPM")->second.getCMD() << std::endl;
	}

	return 0;
}