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
	//Variable para cargar información de configuración
	cfgType variablesCfg;
	//Asigna los valores del archivo de configuración a la variable
	loadCfg("./configuration.cfg", &variablesCfg);

	//Intenta conectarse con el dispositivo con el nombre de la configuración
	Obd connection = Obd(variablesCfg["BT-DISP-NAME"].c_str());

	//Si se ha conectado correctamente se pasa a comprobar los DTC's
	if(connection.isValid()){
		debugLog("Se ha conectado correctamente");
		debugLog("Valores pasados:\nIP: %s\nPuerto: %s", variablesCfg["IP"].c_str(), variablesCfg["PORT"].c_str());
		//Configuración de dirección y puerto del servidor para enviar los DTC's
		AlarmFile fileAlr = AlarmFile(variablesCfg["IP"].c_str(), variablesCfg["PORT"].c_str(), variablesCfg["ALARM-FILE-NAME"].c_str(), variablesCfg["ALARM-FILE-NAME"].c_str());
		//Dirección MAC para identificar el vehículo de origen de los datos
		std::string macAddress = getmac(variablesCfg["INTERFACE-NAME"].c_str());

		//Bucle infinito que cada cierto tiempo consulta si hay algún DTC
		while(1){
			//Espera un periodo para volver a consultar DTC
			std::this_thread::sleep_for(std::chrono::seconds(std::stoi(variablesCfg["PERIOD"])));
			//Comprueba si hay DTC en el vehículo
			std::vector<std::string> vecDTCs = connection.getDTCs();
			//ELIMINAR EJEMPLO DE COMANDO SPEED!!!
			//connection.send(connection.map_commands.find("SPEED")->second);

			if(vecDTCs.empty()){
				std::cout << "No hay DTCs" << std::endl;
			} else {
				//Si hay algún DTC se obtiene la geoposición en la que se detectó
				std::string geoPos = fileAlr.getGeoPos(variablesCfg["PORT-GPS"].c_str());
				std::cout << "Códigos de errores DTCs:" << std::endl;
				for (uint32_t i = 0; i < vecDTCs.size(); ++i){
					std::cout <<  "#############################" << std::endl;
					std::cout <<  vecDTCs[i] << std::endl;
					std::cout <<  "#############################" << std::endl;
					//Forma el mensaje con los datos obtenidos
					std::string msgToSend = formMessage(macAddress, geoPos, vecDTCs[i]);
					//Envía la alarma al servidor
					if (fileAlr.sendAlarm(msgToSend)){
						std::cout << "Alarma enviada." << std::endl;
					} else {
						std::cout << "Mensaje vacío, alarma no enviada." << std::endl;
					}					
				}
			}
		}
		//connection.printStatus();
		//connection.printPIDs();
		/*
		if(connection.getVIN().empty()){
			std::cout << "Vehicle Identification Number no disponible" << std::endl;
		} else {
			std::cout << "Vehicle Identification Number = " << connection.getVIN() << std::endl;
		}
		*/
		//connection.disconnectBluetooth();
	} else {
		debugError("No se ha conectado correctamente");
	}

	return 0;
}