#include "Obd.hpp"
#include "picangps.hpp"
#include "alarmfile.hpp"

typedef  std::map <std::string, std::string> cfgType;

//Formato general de envío de datos
std::string formMessage(std::string macAddress, std::string geoPos, std::string DTC){
	std::string msg;

	msg.append(macAddress);
	msg.append(";");
	msg.append(geoPos);
	msg.append(";");
	msg.append(DTC);

	return msg;
}

//Formato JSON para el envío al servidor web
std::string formMessageJSON(std::string macAddress, std::string geoPos, std::string DTC){
	std::string msg;

	json data;

	std::vector<int> coordenadas;
	
	char* geoPosChar = const_cast<char*>(geoPos.c_str());

	for (int i = 0; i < 4; ++i){
		    char* token = strtok(geoPosChar, ",");
    		if(token != NULL){
        		while(token != NULL){
            	// Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
            	coordenadas.push_back(atoi(token));
            	token = strtok(NULL, ",");
        		}
    		}
	}

	time_t now = time(0);
	char* dt = ctime(&now);

	data["date"] = dt;
	data["coordinates"] = coordenadas;
	data["vehicle"] = macAddress;
	data["code"] = DTC;

	//Web que nos proporciona información sobre el DTC y acciones a realizar
	std::string enlace = "https://codigosdtc.com/" + DTC;

	data["description"] = enlace;

	//Formato de JSON sin espacios con más eficiencia en el envío de datos
	msg = data.dump();

	return msg;
}

int main()
{
	//Variable para cargar información de configuración
	cfgType variablesCfg;
	//Asigna los valores del archivo de configuración a la variable
	loadCfg("./conf/configuration.cfg", &variablesCfg);

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
					std::string msgToSend = formMessageJSON(macAddress, geoPos, vecDTCs[i]);
					//Envía la alarma al servidor
					if (fileAlr.sendAlarm(msgToSend)){
						std::cout << "Alarma enviada." << std::endl;
					} else {
						std::cout << "Mensaje vacío, alarma no enviada." << std::endl;
					}					
				}
			}
		}
	} else {
		debugError("No se ha conectado correctamente");
	}

	return 0;
}