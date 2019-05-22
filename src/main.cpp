#include "Obd.hpp"
#include "picangps.hpp"
#include "alarmfile.hpp"

typedef  std::map <std::string, std::string> cfgType;

int main(int argc, char **argv)
{
	cfgType variablesCfg;
	loadCfg("./configuration.cfg", &variablesCfg);
	//std::cout << variablesCfg["BT-DISP-NAME"] << std::endl;

	Obd connection = Obd(variablesCfg["BT-DISP-NAME"].c_str());

	if(connection.isValid()){
		std::cout << "Se ha conectado correctamente" << std::endl;

		//connection.send(connection.map_commands.find(argv[2])->second);

		std::vector<std::string> vecDTCs = connection.getDTCs();

		if(vecDTCs.empty()){
			std::cout << "No hay DTCs" << std::endl;
		} else {
			std::cout << "Códigos de errores DTCs:" << std::endl;
			for (uint32_t i = 0; i < vecDTCs.size(); ++i){
				std::cout <<  vecDTCs[i] << std::endl;
			}
		}

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