#include "Obd.hpp"
#include "picangps.hpp"
#include "alarmfile.hpp"

typedef  std::map <std::string, std::string> cfgType;

std::string formMessage(std::string macAddress, std::string geoPos, std::string DTC){
	std::string msg;

	msg.append(macAddress);
	msg.append(";");
	msg.append(geoPos);
	msg.append(";");
	msg.append(DTC);
	//msg.append(";");

	return msg;
}

int main(int argc, char **argv)
{
	cfgType variablesCfg;
	loadCfg("./configuration.cfg", &variablesCfg);
	//std::cout << variablesCfg["BT-DISP-NAME"] << std::endl;

	Obd connection = Obd(variablesCfg["BT-DISP-NAME"].c_str());

	if(connection.isValid()){
		std::cout << "Se ha conectado correctamente" << std::endl;
		AlarmFile fileAlr = AlarmFile(variablesCfg["IP"], variablesCfg["PORT"], variablesCfg["ALARM-FILE-NAME"], variablesCfg["ALARM-FILE-NAME"]);
		std::string macAddress = getmac(variablesCfg["INTERFACE-NAME"].c_str());
		//connection.send(connection.map_commands.find(argv[2])->second);
		while(1){
			std::this_thread::sleep_for(std::chrono::seconds(std::stoi(variablesCfg["PERIOD"])));
			std::vector<std::string> vecDTCs = connection.getDTCs();
			//connection.send(connection.map_commands.find("SPEED")->second);

			if(vecDTCs.empty()){
				std::cout << "No hay DTCs" << std::endl;
			} else {
				std::string geoPos = fileAlr.getGeoPos(variablesCfg["PORT-GPS"]);
				std::cout << "Códigos de errores DTCs:" << std::endl;
				for (uint32_t i = 0; i < vecDTCs.size(); ++i){
					std::cout <<  "#############################" << std::endl;
					std::cout <<  vecDTCs[i] << std::endl;
					std::cout <<  "#############################" << std::endl;
					std::string msgToSend = formMessage(macAddress, geoPos, vecDTCs[i]);
					if (fileAlr.sendAlarm(msgToSend)){
						std::cout << "Alarma enviada." << std::endl;
					} else {
						std::cout << "Mensaje vacío, alarma no enviada." << std::endl;
					}					
				}
				//sleep(std::stoi(variablesCfg["PERIOD"]));
			}
		}
		//connection.printStatus();


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