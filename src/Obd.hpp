#ifndef OBD_HPP
#define OBD_HPP

#include <iostream>
#include <fstream>
#include <thread>

#include <bitset>
#include <vector>
#include <sstream>
#include <algorithm>

#include <utility>
#include <map>

#include <typeinfo>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <unistd.h>

#include <ctime>

#include "Commands.hpp"
#include "decoders.hpp"
#include "loadcfg.hpp"
#include "debug.hpp"

#ifdef TEST
	#define socket mock_socket
	#include "../test/MockSocket.cpp"
#endif

#define MAX_EP_EVTS 20

using json = nlohmann::json;

typedef std::pair<std::string, Commands> tupla;

class Obd {
public:
	std::map<std::string, Commands> map_commands;
      // Constructor
	Obd(const char *deviceName){
		// Comenzamos el descubrimiento del dipositivo Bluetooth
		debugLog("Iniciando descubrimiento del dispositivo %s", deviceName);
		this->discoverDeviceAddress(deviceName, this->dest);
		if(this->m_deviceFound){
			// Si lo encontramos nos conectamos
			this->connectBluetooth();
			if (this->m_status){
				// Si la conexión tiene éxito, iniciamos los decodificadores
				this->initDecoderFunctions();
				// Leemos el archivo de PIDS
				this->readFileData();
				// Comenzamos el envío de mensajes de inicio
				this->initMessages();
			}
		} else {
			debugLog("Dispositivo %s no encontrado.", deviceName);
		}
	}

	void connectBluetooth(){
		try{
			struct sockaddr_rc addr;
			int statusConnection;

			// Abrimos socket bluetooh
			this->m_cli_s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

			debugLog("socket: %d", this->m_cli_s);
			if (this->m_cli_s < 0) {
				throw std::string("error abriendo socket BT/RFCOMM");
			}

			addr.rc_family = AF_BLUETOOTH;
			str2ba(this->dest, &addr.rc_bdaddr );
			addr.rc_channel = (uint8_t) 1;

			debugLog("Conectando con %s (canal %d)", this->dest, addr.rc_channel);
			//Iniciamos la conexión
			statusConnection = connect(this->m_cli_s, (struct sockaddr *)&addr, sizeof(addr));

			if (statusConnection) {
				close(this->m_cli_s);
				throw std::string("No se ha podido conectar");
			}

			debugLog("Conectado!");
			this->m_status = true;

			//Creamos instancia epoll para la recepción de datos en el socket
			this->epoll_fd = epoll_create(1);
			if (this->epoll_fd < 0) {
				perror("No se ha podido crear epoll");
				close(this->m_cli_s);
			}

			this->ev.events = EPOLLIN;
			this->ev.data.fd = this->m_cli_s;

			//Añadimos el socket de conexión a la instancia de epoll creada
			int err = epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->m_cli_s, &ev);
			
			if (err) {
				perror("No se ha podido añadir el socket cliente a la instancia epoll");
				close(this->m_cli_s);
				close(this->epoll_fd);
			}


		} catch(std::string e) {
			std::cerr << e << std::endl;
		}
	}

	void readFileData(){
		std::ifstream ifs("data/PIDS.json");
		auto j = json::parse(ifs);

		//Convertimos todos los PIDS en objetos del tipo Commands y los añadimos a Obd
		for (int i = 0; i < (int)j.size(); ++i)
		{
			this->map_commands.insert(tupla(j[i]["name"], Commands(j[i])));
		}
	}

	void discoverDeviceAddress(const char * deviceName, char *deviceAddress){
		inquiry_info *ii = NULL;
		int max_rsp, num_rsp;
		int dev_id, sock, len, flags;
		int i;
		char addr[19] = { 0 };
		char name[248] = { 0 };

		//Identificamos la interfaz bluetooth del dispositivo
		dev_id = hci_get_route(NULL);
		//Abrimos socket para esta interfaz
		sock = hci_open_dev( dev_id );
		if (dev_id < 0 || sock < 0) {
			perror("Abriendo socket");
			exit(1);
		}

		len  = 8;
		max_rsp = 255;
		flags = IREQ_CACHE_FLUSH;
		ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

		//Iniciamos el descubrimiento de dispositivos bluetooth
		num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
		if( num_rsp < 0 ) perror("hci_inquiry");

		//Entre todas las respuestas buscamos el dispositivo bluetooth de OBDII
		for (i = 0; i < num_rsp; i++) {
			ba2str(&(ii+i)->bdaddr, addr);
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
				strcpy(name, "[unknown]");
			debugLog("%s  %s", addr, name);
			//Si la cadena introducida a la función es igual al dispositivo encontrado guardamos la dirección
			if(strcmp(deviceName, name) == 0){
				this->m_deviceFound = true;
				strcpy(deviceAddress, addr);
				debugLog("Dispositivo %s encontrado", deviceName);
				break;
			}
		}

		free( ii );
		close( sock );
	}	
	
	void send(Commands command){
		//Iniciamos en un hilo de ejecución la función polling de recepción de datos
		std::thread t1(&Obd::polling, this, command);

		char *p;
		char buf[1024];
		int len;

		//Comando a enviar
		std::string message = command.getCMD();
		strcpy(buf, message.c_str());

		len = strlen(buf);
		buf[len] = '\n';
		buf[len+1] = '\0';
		
		// Todo los mensajes a ELM327  deben terminar con el caracter retorno de carro (hex  ‘0D’, \r).
		p = buf;
		while (*p) {
			if (*p == '\n')
				*p = '\r';
			p++;
		}
		
		debugLog("Mensaje a enviar: %s", buf);
		debugLog("Enviando mensaje...");
		write(this->m_cli_s, buf, strlen(buf));
		
		//Queda a la espera de finalización de ejecución del hilo de recepción del mensaje OBD
		t1.join();
	}

	void polling(Commands command){
		struct epoll_event events[MAX_EP_EVTS];
		int nfds;
		bool continuar = true;

		debugLog("Polling function");

		// Bucle infinito para el envío de datos por bluetooth al conector OBD
		while(continuar) {
		// Buffer para enviar y recibir
			char message_rcv[1024], buf[1024], *p;
			ssize_t len;
			int i;
			//Quedamos a la espera de recepción de eventos en la instancia epoll (socket)
			nfds = epoll_wait(this->epoll_fd, events, MAX_EP_EVTS, -1);
			if (nfds < 0) {
				perror("epoll error");
				break;
			}
			for (i = 0; i < nfds; i++) {
				if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
					debugError("epoll error");
				}
				//Si los eventos detectados corresponden al socket de conexión con el vehículo, tratamos el mensaje
				if (events[i].data.fd == this->m_cli_s) {
					len = read(this->m_cli_s, &buf, sizeof(buf) - 1);
					if (len < 0) {
						perror("socket read error");
						continue;
					}
					debugLog("Evento leído: %s", buf);
					strcat(message_rcv, buf);
					//Si se detecta el caracter ">" se ha finalizado el mensaje
					if(strstr(buf, ">") != NULL) {
						len = strlen(message_rcv);
						message_rcv[len] = '\0';

						p = message_rcv;
						//Conversión inversa del mensaje ELM327 enviado en el último carácter
						while(*p) {
							if (*p == '\r')
								*p = '\n';
							p++;
						}
						//Transformar respuesta
						debugLog("Mensaje recibido:\n%s", message_rcv);

						char * ocurrencia = message_rcv;
						if((ocurrencia=strstr(ocurrencia, command.getCMDResponse().c_str())) != NULL){
							while((ocurrencia=strstr(ocurrencia, command.getCMDResponse().c_str())) != NULL){
								debugLog("Ocurrencia encontrada");
								char info[1024];
								memset(info, '\0', sizeof(info));
								strncpy(info, ocurrencia + command.getCMD().size() , command.getBytesResponse());
								debugLog("Información: %s", info);
								std::string type_data = command.getTypeData();
								//Dependiendo del tipo de dato de la respuesta se busca el decodificador correspondiente
								if (!type_data.compare("float")){
									auto varResultado = this->decoderFunctionsFloat[command.getDecoder().c_str()](info);
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									std::cout << "-> " << varResultado << " "<< command.getUnits() << std::endl;
									this->map_commands.find(command.getName())->second.setResValue(varResultado);
								} else if(!type_data.compare("OxigenoResponse")){
									auto varResultado = this->decoderFunctionsStructOx[command.getDecoder().c_str()](info);
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									std::cout << "-> " << varResultado.A << "/" << varResultado.B << " "<< command.getUnits() << std::endl;
									this->map_commands.find(command.getName())->second.setResValue(varResultado);

								} else if (!type_data.compare("RelacionesResponse")) {
									auto varResultado = this->decoderFunctionsStructRel[command.getDecoder().c_str()](info);
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									std::cout << "-> " << varResultado.A << "/" << varResultado.B << "/" << varResultado.C << "/" << varResultado.D << " "<< command.getUnits() << std::endl;
									this->map_commands.find(command.getName())->second.setResValue(varResultado);

								} else if(!type_data.compare("vectorInt")){
									auto varResultado = this->decoderFunctionsVectorInt[command.getDecoder().c_str()](info);
									this->map_commands.find(command.getName())->second.setResValue(varResultado);

									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									//Tratamiento para los PIDs disponibles
									for (uint32_t i = 0; i < varResultado.size(); ++i){
										std::string substr_cmd = command.getCMD().substr(2,2);
										int sum_pid = stoi(substr_cmd,nullptr,16);
										std::stringstream stream;
										stream << std::hex << sum_pid+varResultado[i];
										std::string result(stream.str());
										if(result.size() == 1)
										//Si el resultado solo tiene un caracter se añade un 0 al principio
											result.insert(0,"0");
										result.insert(0,"01");
										std::transform(result.begin(), result.end(),result.begin(), ::toupper);

										//Almacenamos el resultado de los PIDs disponibles
										this->vecPIDs.push_back(result);
									}
								} else if (!type_data.compare("vectorStr")) {
									auto varResultado = this->decoderFunctionsVectorStr[command.getDecoder().c_str()](info);
									this->map_commands.find(command.getName())->second.setResValue(varResultado);
									//Decodificador para DTC
									if (varResultado.empty()){
										debugLog("No hay DTC en el vehículo");
									} else {
										this->vecDTCs = varResultado;
										for (uint32_t i = 0; i < varResultado.size(); ++i)
										{
											debugLog("Enviar DTC: %s", varResultado[i].c_str());
										}
									}
								} else if (!type_data.compare("string")) {
									auto varResultado = this->decoderFunctionsStr[command.getDecoder().c_str()](info);
									this->map_commands.find(command.getName())->second.setResValue(varResultado);
									//Decodificador para el número de identificación del vehículo
									if(!command.getDecoder().compare("decodeVIN"))
										this->vin.append(varResultado);
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
								} else if (!type_data.compare("map")) {
									auto varResultado = this->decoderFunctionsMap[command.getDecoder().c_str()](info);
									this->map_commands.find(command.getName())->second.setResValue(varResultado);
									this->mapStatus = varResultado;
								} else {
									debugLog("Tipo de dato no reconocido");
								}
								ocurrencia++;
							}
							std::cout << "--------------------------------------------------------------" << std::endl;
							memset(message_rcv, '\0', sizeof(message_rcv));
							continuar = false;
							//Respuestas de mensajes de AT de configuración
						} else if((strstr(message_rcv, "OK")) != NULL){
							debugLog("%s = OK.", command.getDescription().c_str());
							memset(message_rcv, '\0', sizeof(message_rcv));							
							continuar = false;
							//Vehículo sin el dato solicitado
						} else if((strstr(message_rcv, "NO DATA")) != NULL){
							debugLog("%s = No disponible.", command.getDescription().c_str());
							memset(message_rcv, '\0', sizeof(message_rcv));							
							continuar = false;
						} else {
							//Para conocer el protocolo actual
							if(!command.getName().compare("DESCRIBE_PROTOCOL")){
								char info[1024];
								char* token = strtok(message_rcv, "\n");
								strcpy(info, token);
								auto varResultado = this->decoderFunctionsStr[command.getDecoder().c_str()](info);
								this->map_commands.find(command.getName())->second.setResValue(varResultado);
								this->currentProtocol = varResultado;
							}else if(!command.getName().compare("DESCRIBE_PROTOCOL_NUMBER")){
								char info[1024];
								char* token = strtok(message_rcv, "\n");
								strcpy(info, token);
								auto varResultado = this->decoderFunctionsStr[command.getDecoder().c_str()](info);
								this->map_commands.find(command.getName())->second.setResValue(varResultado);
								this->currentProtocolNumber = varResultado;
							} else {
								debugLog("Mensaje recibido no entendido!");
							}
							memset(message_rcv, '\0', sizeof(message_rcv));							
							continuar = false;
						}
					}

					memset(buf, '\0', sizeof(buf));
				} else {
					debugError("Evento desconocido");
				}
			}
		}
	}

	void initMessages(){
		//Inicialización de la conexión con ELM327
		std::map<std::string, std::string> listPIDs = {
			{"PIDS_B", "0120"},
			{"PIDS_C", "0140"},
			{"PIDS_D", "0160"},
			{"PIDS_E", "0180"},
			{"PIDS_F", "01A0"},
			{"PIDS_G", "01C0"}
		};
		this->send(this->map_commands.find("RESET")->second);
		this->send(this->map_commands.find("DEFAULT_VALUES")->second);
		this->send(this->map_commands.find("RESP_SIN_ESPACIOS")->second);					
		this->send(this->map_commands.find("SIN_ECO")->second);
		this->send(this->map_commands.find("SIN_HEADER")->second);					
		this->send(this->map_commands.find("AUTO_PROTO")->second);
		this->send(this->map_commands.find("STATUS")->second);
		this->send(this->map_commands.find("GET_VIN")->second);
		this->send(this->map_commands.find("PIDS_A")->second);
		//Bucle para detectar PIDs disponibles
		for (std::map<std::string, std::string>::iterator it=listPIDs.begin(); it!=listPIDs.end(); ++it){
			if(this->existPID(it->second)){
				this->send(this->map_commands.find(it->first)->second);	
			}
		}
		debugLog("Nº de comandos disponibles = %zu", vecPIDs.size());
	}

	void initDecoderFunctions(){
	//Inicia las funciones dependiendo del tipo de dato de respuesta	
	this->decoderFunctionsFloat = {
			{ "decodeCargaPosicionEGR", decodeCargaPosicionEGR},
			{ "decodeTempGeneral", decodeTempGeneral},
			{ "decodeAjusteCombustibleEGR", decodeAjusteCombustibleEGR},
			{ "decodePresionCombustible", decodePresionCombustible},
			{ "decodeHexToDec", decodeHexToDec},
			{ "decodeRPM", decodeRPM},
			{ "decodeAvanceTiempo", decodeAvanceTiempo},
			{ "decodeVelocidadMAF", decodeVelocidadMAF},
			{ "decodePresionCombColector", decodePresionCombColector},
			{ "decodePresionMedidorCombustible", decodePresionMedidorCombustible},
			{ "decodePresionVapor", decodePresionVapor},
			{ "decodeTempCatalizador", decodeTempCatalizador},
			{ "decodeVoltajeControl", decodeVoltajeControl},
			{ "decodeRelacionCombAireBasica", decodeRelacionCombAireBasica}
        };
		this->decoderFunctionsStructOx = {
			{ "decodeSensorOxigeno", decodeSensorOxigeno},
			{ "decodeRelacionCombAire", decodeRelacionCombAire},
			{ "decodeRelacionCombAireActual", decodeRelacionCombAireActual}
		};

		this->decoderFunctionsStructRel["decodeRelaciones"] = decodeRelaciones;
		this->decoderFunctionsVectorInt["decodePIDS"] = decodePIDS;
		this->decoderFunctionsVectorStr["decodeDTCs"] = decodeDTCs;
		this->decoderFunctionsStr = {
			{"decodeVIN", decodeVIN},
			{"decodeDescribeProtocol", decodeDescribeProtocol}
		};
		this->decoderFunctionsMap["decodeStatus"] = decodeStatus;
		this->noDecodeFunctionAT["noDecodeAT"] = noDecodeAT;
	}

	void disconnectBluetooth(){
		debugLog("Desconectando dispositivo Bluetooth");
		close(this->m_cli_s);
		close(this->epoll_fd);
	}

	bool existPID(std::string command){
		bool exists = false;
		//Comprueba en la lista de PIDs que el comando este implementado
		for (uint32_t i = 0; i < this->vecPIDs.size(); ++i){
			if(!this->vecPIDs[i].compare(command)){
				exists = true;
				break;
			}
		}
		return exists;
	}
	

	void printPIDs(){
		//Iteración para imprimir por consola los PIDs disponibles
		for (std::map<std::string, Commands>::iterator it=this->map_commands.begin(); it!=this->map_commands.end(); ++it){
			Commands command = it->second;
			std::string str_cmd = command.getCMD();
			for (uint32_t i = 0; i < this->vecPIDs.size(); ++i){
				if(!str_cmd.compare(this->vecPIDs[i])){
					std::cout << str_cmd << " - "<< command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
					break;
				}
			}
		}
	}



	void printStatus(){
		//Imprime por consola los resultados de las pruebas del comando STATUS
		for (std::map<std::string, std::string>::iterator it=this->mapStatus.begin(); it!=this->mapStatus.end(); ++it){
			std::cout << it->first << " -> " << it->second << std::endl;
		}
	}

	std::string getVIN(){
		//Devuelve el número de identificación del vehículo
		return this->vin;
	}
	
	std::vector<std::string> getDTCs(){
		time_t curr_time;
		tm *curr_tm;
		char date_string[100];
		char time_string[100];
		time(&curr_time);

		curr_tm = localtime(&curr_time);
		strftime(date_string, 50, "%d/%m/%Y", curr_tm);
		strftime(time_string, 50, "%T", curr_tm);
		std::cout << date_string << " " << time_string << std::endl;

		//Consulta de DTC del vehículo
		this->send(this->map_commands.find("STATUS")->second);
		if (this->mapStatus["DTC_CNT"].compare("0")){
			this->send(this->map_commands.find("GET_DTC")->second);
		} else {
			std::cout << "No hay DTC disponibles" << std::endl;
		}
		return this->vecDTCs;
	}


	bool isValid(){
		//Bool del estado de la conexión bluetooth
		return this->m_status;
	}
private:
    // Atributos privados de la clase "Obd"
    //Vector con PIDs disponibles
	std::vector<std::string> vecPIDs;
	//Vector con DTCs activos
	std::vector<std::string> vecDTCs;
	//Cadena VIN
	std::string vin;
	//Cadena con el protocolo actual de comunicación
	std::string currentProtocol;
	//Cadena con el número del protocolo actual de comunicación
	std::string currentProtocolNumber;
	//Map con los valores de las pruebas realizadas en el vehículo
	std::map<std::string, std::string> mapStatus;

	//Maps para decodificadores
	std::map<std::string, std::function<void()>> noDecodeFunctionAT;
	std::map<std::string, std::function<float(char *)>> decoderFunctionsFloat;
	std::map<std::string, std::function<struct OxigenoResponse(char *)>> decoderFunctionsStructOx;
	std::map<std::string, std::function<struct RelacionesResponse(char *)>> decoderFunctionsStructRel;
	std::map<std::string, std::function<std::vector<int>(char *)>> decoderFunctionsVectorInt;
	std::map<std::string, std::function<std::vector<std::string>(char *)>> decoderFunctionsVectorStr;
	std::map<std::string, std::function<std::string(char *)>> decoderFunctionsStr;
	std::map<std::string, std::function<std::map<std::string, std::string>(char *)>> decoderFunctionsMap;

	//Cadena de identificación del dispositivo físico bluetooth
	char dest[19] = { 0 };
	//Socket de comunicación
	int m_cli_s;
	//Booleanos de estado de la conexión del dispositivo bluetooth
	bool m_deviceFound = false;
	bool m_status = false;
	//Variables de control del socket de conexión OBD
	int epoll_fd;
	struct epoll_event ev;
};


#endif