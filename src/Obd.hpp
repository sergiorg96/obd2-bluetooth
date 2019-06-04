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

#define MAX_EP_EVTS 20

using json = nlohmann::json;

typedef std::pair<std::string, Commands> tupla;

class Obd {
public:
	std::map<std::string, Commands> map_commands;
      // Constructor
	Obd(const char *deviceName){
		this->discoverDeviceAddress(deviceName, this->dest);
		if(this->m_deviceFound){
			this->connectBluetooth();
			if (this->m_status){
				this->initDecoderFunctions();
				this->readFileData();
				this->initMessages();
			}
		} else {
			printf("Device %s not found.\n", deviceName);
		}
	}
      // Funciones miembro de la clase "Obd"
      //void getDato();
      //void setDato(int dato);
	void connectBluetooth(){
		try{
			struct sockaddr_rc addr;
			int statusConnection;
			//char dest[18] = "00:1D:A5:00:0D:AC";


			this->m_cli_s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
			if (this->m_cli_s < 0) {
				throw std::string("error opening BT/RFCOMM socket");
			}
			//Faltaría sustituir automaticamente la MAC
			addr.rc_family = AF_BLUETOOTH;
			str2ba(this->dest, &addr.rc_bdaddr );
			addr.rc_channel = (uint8_t) 1;

			printf("connecting to %s (channel %d)\n", this->dest, addr.rc_channel);

			// connect to server, throw socket s (m_cli_s -> descriptor)
			statusConnection = connect(this->m_cli_s, (struct sockaddr *)&addr, sizeof(addr));

			if (statusConnection) {
				close(this->m_cli_s);
				throw std::string("unable to connect");
			}

			printf("connected\n");
			this->m_status = true;

			this->epoll_fd = epoll_create(1);
			std::cout << this->epoll_fd << std::endl;
			if (this->epoll_fd < 0) {
				perror("Unable to create epoll");
				close(this->m_cli_s);
			}

			this->ev.events = EPOLLIN;
			this->ev.data.fd = this->m_cli_s;

			int err = epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->m_cli_s, &ev);
			
			//std::cout << EBADF << "," << EEXIST << "," << EINVAL << "," << ELOOP << "," << ENOENT << "," << ENOMEM << ",";
			//std::cout << ENOSPC << "," << EPERM << std::endl;  
			if (err) {
				perror("unable to add client socket to epoll instance");
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

		dev_id = hci_get_route(NULL);
		sock = hci_open_dev( dev_id );
		if (dev_id < 0 || sock < 0) {
			perror("opening socket");
			exit(1);
		}

		len  = 8;
		max_rsp = 255;
		flags = IREQ_CACHE_FLUSH;
		ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

		num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
		if( num_rsp < 0 ) perror("hci_inquiry");

		for (i = 0; i < num_rsp; i++) {
			ba2str(&(ii+i)->bdaddr, addr);
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
				name, 0) < 0)
				strcpy(name, "[unknown]");
			printf("%s  %s\n", addr, name);
			//Si la cadena introducida a la función es igual al dispositivo encontrado guardamos la dirección
			if(strcmp(deviceName, name) == 0){
				this->m_deviceFound = true;
				strcpy(deviceAddress, addr);
				printf("Dispositivo %s encontrado\n", deviceName);
				break;
			}
		}

		free( ii );
		close( sock );
	}
	
	void send(Commands command){

		//printf("Mensaje a enviar: %s\n", message);

		std::thread t1(&Obd::polling, this, command);

		char *p;
		char buf[1024];
		int len;

		//std::cout << "Esperamos 2 segundos..." << std::endl;
		//std::this_thread::sleep_for(std::chrono::seconds(2));
		std::cout << "Enviando mensaje..." << std::endl;

		std::string message = command.getCMD();


		strcpy(buf, message.c_str());

		len = strlen(buf);
		//strcpy(buf, "010C\n");



			//Mirar si sustituir \n por \r directamente
		buf[len] = '\n';
		buf[len+1] = '\0';
		
			/* All  messages  to  the ELM327  must  be
			 * terminated  with  a  carriage  return
			 * character  (hex  ‘0D’, \r).
			 */
		
		p = buf;
		while (*p) {
			if (*p == '\n')
				*p = '\r';
			p++;
		}
		
		printf("Mensaje a enviar: %s\n", buf);
			//printf("Longitud: %d %d\n", (int) strlen(buf), len);

		write(this->m_cli_s, buf, strlen(buf));
		
			//std::cout << "Mensaje " << message << " enviado" << std::endl;
		t1.join();
	}

	void polling(Commands command){
		struct epoll_event events[MAX_EP_EVTS];
		/*
		struct epoll_event {
        	uint32_t     events;    Epoll events 
        	epoll_data_t data;};      User data variable 

    	typedef union epoll_data {
        	void    *ptr;
        	int      fd;
        	uint32_t u32;
        	uint64_t u64;
    	} epoll_data_t;
    	The data field of each returned structure contains the same data as
       	was specified in the most recent call to epoll_ctl(2) (EPOLL_CTL_ADD,
       	EPOLL_CTL_MOD) for the corresponding open file description.  The
       	events field contains the returned event bit field.
		*/

		int nfds;
		//int fd_out = fileno(stdout);

		bool continuar = true;
		//El parametro de epoll_create significa el número de file descriptor que un proceso quiere monitorizar
		// y ayuda al Kernel a decidir el tamaño de la instancia epoll.
		/*
		epoll_fd = epoll_create(1);
		std::cout << epoll_fd << std::endl;
		if (epoll_fd < 0) {
			perror("Unable to create epoll");
			close(this->m_cli_s);
			return epoll_fd;
		}
		*/

		/*
		Estructura Epoll:
		Like so, if fd is a socket, we might want to monitor it for the arrival of new data on the socket buffer (EPOLLIN).
		*/
		//ev.events = EPOLLIN;
		// Se almacena el file descriptor en el campo data de epoll_event
		//ev.data.fd = this->m_cli_s;
		/*
		A process can add file descriptors it wants monitored to the epoll instance by calling epoll_ctl.
 		All the file descriptors registered with an epoll instance are collectively called an epoll set or the interest list.
		int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
		*/
		// Se añade al epoll set o interest list, el descriptor de la conexión del socket
		/*
		err = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->m_cli_s, &ev);
		std::cout << "Error: " << errno << std::endl;
		std::cout << EBADF << "," << EEXIST << "," << EINVAL << "," << ELOOP << "," << ENOENT << "," << ENOMEM << ",";
		std::cout << ENOSPC << "," << EPERM << std::endl;  
		if (err) {
			perror("unable to add client socket to epoll instance");
			close(this->m_cli_s);
			close(epoll_fd);
			return err;
		}
		*/
		printf("Polling function\n");
		// Bucle infinito para el envío de datos por bluetooth al conector OBD
		while(continuar) {
		// Buffer para enviar y recibir
			char message_rcv[1024], buf[1024], *p;
			ssize_t len;
			int i;
		/*
		A thread can be notified of events that happened on the epoll set/interest set of an epoll instance by calling the epoll_wait system call,
 		which blocks until any of the descriptors being monitored becomes ready for I/O.
 		int epoll_wait(int epfd, struct epoll_event *evlist, int maxevents, int timeout);

 		evlist - is an array of epoll_event structures. evlist is allocated by the calling process and when epoll_wait returns,
 		this array is modified to indicate information about the subset of file descriptors in the interest list that
 		 are in the ready state (this is called the ready list)

 		 timeout — this argument behaves the same way as it does for poll or select.
 		  This value specifies for how long the epoll_wait system call will block:
 		  -1 -> when timeout is set to -1, epoll_wait will block “forever”.
		*/
			//nfds = epoll_wait(epoll_fd, events, MAX_EP_EVTS, -1);
			nfds = epoll_wait(this->epoll_fd, events, MAX_EP_EVTS, -1);			
			if (nfds < 0) {
				perror("epoll error");
				break;
			}
			//printf("Se ha detectado un evento(nfds): %d\n", nfds);
			for (i = 0; i < nfds; i++) {
				if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
					fprintf(stderr, "epoll error\n");
				}

				if (events[i].data.fd == this->m_cli_s) {
					len = read(this->m_cli_s, &buf, sizeof(buf) - 1);
					if (len < 0) {
						perror("socket read error");
						continue;
					}
					//printf("RECIBIDO: %s\n", buf);
					strcat(message_rcv, buf);
					if(strstr(buf, ">") != NULL) {
						len = strlen(message_rcv);
						message_rcv[len] = '\0';

						p = message_rcv;
						while(*p) {
							if (*p == '\r')
								*p = '\n';
							p++;
						}
						//Transformar respuesta
						//char *ocurrencia = strstr(message_rcv, "410D");
						printf("Mensaje recibido:\n%s", message_rcv);

						//char *ocurrencia = strstr(message_rcv, command.getCMDResponse().c_str());
						char * ocurrencia = message_rcv;
						//printf("He recibido: %s\n", message_rcv);
						if((ocurrencia=strstr(ocurrencia, command.getCMDResponse().c_str())) != NULL){
							while((ocurrencia=strstr(ocurrencia, command.getCMDResponse().c_str())) != NULL){
								printf("Ocurrencia encontrada\n");
								char info[1024];
								memset(info, '\0', sizeof(info));
								strncpy(info, ocurrencia + command.getCMD().size() , command.getBytesResponse());
								printf("Info: %s\n", info);
								std::string type_data = command.getTypeData();
								//HAY QUE CAMBIAR DECODERFUNCTIONS
								if (!type_data.compare("float")){
									auto varResultado = this->decoderFunctionsFloat[command.getDecoder().c_str()](info);
									//std::cout << "Tipo de dato: "<< typeid(varResultado).name() << std::endl;
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									std::cout << "-> " << varResultado << " "<< command.getUnits() << std::endl;
								} else if(!type_data.compare("OxigenoResponse")){
									auto varResultado = this->decoderFunctionsStructOx[command.getDecoder().c_str()](info);
									//std::cout << "Tipo de dato: "<< typeid(varResultado).name() << std::endl;
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									std::cout << "-> " << varResultado.A << "/" << varResultado.B << " "<< command.getUnits() << std::endl;
								} else if (!type_data.compare("RelacionesResponse")) {
									auto varResultado = this->decoderFunctionsStructRel[command.getDecoder().c_str()](info);
									//std::cout << "Tipo de dato: "<< typeid(varResultado).name() << std::endl;
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
									std::cout << "-> " << varResultado.A << "/" << varResultado.B << "/" << varResultado.C << "/" << varResultado.D << " "<< command.getUnits() << std::endl;
								} else if(!type_data.compare("vectorInt")){
									auto varResultado = this->decoderFunctionsVectorInt[command.getDecoder().c_str()](info);
									//std::cout << "Tipo de dato: "<< typeid(varResultado).name() << std::endl;
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
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

										this->vecPIDs.push_back(result);
									}
								} else if (!type_data.compare("vectorStr")) {
									auto varResultado = this->decoderFunctionsVectorStr[command.getDecoder().c_str()](info);
									if (varResultado.empty()){
										std::cout << "No hay DTC en el vehículo" << std::endl;
									} else {
										this->vecDTCs = varResultado;
										for (uint32_t i = 0; i < varResultado.size(); ++i)
										{
											std::cout << "Enviar " << varResultado[i] << std::endl; 
										}
									}
								} else if (!type_data.compare("string")) {
									auto varResultado = this->decoderFunctionsStr[command.getDecoder().c_str()](info);
									if(!command.getDecoder().compare("decodeVIN"))
										this->vin.append(varResultado);
									//std::cout << "Tipo de dato: "<< typeid(varResultado).name() << std::endl;
									std::cout << command.getName() << " - " << command.getDescription() << " - Min=" << command.getMIN() << " Max=" << command.getMAX() << std::endl;
								} else if (!type_data.compare("map")) {
									auto varResultado = this->decoderFunctionsMap[command.getDecoder().c_str()](info);
									this->mapStatus = varResultado;
									//std::cout << "Tipo de dato: "<< typeid(varResultado).name() << std::endl;
								} else {
									std::cout << "Tipo de dato no reconocido" << std::endl;
								}
								ocurrencia++;
							}
							std::cout << "--------------------------------------------------------------" << std::endl;
							memset(message_rcv, '\0', sizeof(message_rcv));
							continuar = false;
						} else if((strstr(message_rcv, "OK")) != NULL){
							std::cout << command.getDescription() << " = OK." << std::endl;
							memset(message_rcv, '\0', sizeof(message_rcv));							
							continuar = false;
						} else if((strstr(message_rcv, "NO DATA")) != NULL){
							std::cout << command.getDescription() << " = No disponible." << std::endl;
							memset(message_rcv, '\0', sizeof(message_rcv));							
							continuar = false;
						} else {
							//Para conocer el protocolo actual
							if(!command.getName().compare("DESCRIBE_PROTOCOL")){
								char info[1024];
								char* token = strtok(message_rcv, "\n");
								strcpy(info, token);
								auto varResultado = this->decoderFunctionsStr[command.getDecoder().c_str()](info);
								this->currentProtocol = varResultado;
							}else if(!command.getName().compare("DESCRIBE_PROTOCOL_NUMBER")){
								char info[1024];
								char* token = strtok(message_rcv, "\n");
								strcpy(info, token);
								auto varResultado = this->decoderFunctionsStr[command.getDecoder().c_str()](info);
								this->currentProtocolNumber = varResultado;
							} else {
								printf("Mensaje recibido no entendido!\n");
							}
							memset(message_rcv, '\0', sizeof(message_rcv));							
							continuar = false;
						}
					}

					memset(buf, '\0', sizeof(buf));
				} else {
					fprintf(stderr, "unknown event");
				}
			}
		}
	}

	void initMessages(){
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
		for (std::map<std::string, std::string>::iterator it=listPIDs.begin(); it!=listPIDs.end(); ++it){
			if(this->existPID(it->second)){
				this->send(this->map_commands.find(it->first)->second);	
			}
		}
		std::cout << "Nº de comandos disponibles = " << vecPIDs.size() << std::endl;
	}

	void initDecoderFunctions(){
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
		std::cout << "Desconectando dispositivo Bluetooth" << std::endl;
		close(this->m_cli_s);
		close(this->epoll_fd);
	}

	bool existPID(std::string command){
		bool exists = false;
		for (uint32_t i = 0; i < this->vecPIDs.size(); ++i){
			if(!this->vecPIDs[i].compare(command)){
				exists = true;
				break;
			}
		}
		return exists;
	}
	

	void printPIDs(){
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
		for (std::map<std::string, std::string>::iterator it=this->mapStatus.begin(); it!=this->mapStatus.end(); ++it){
			std::cout << it->first << " -> " << it->second << std::endl;
		}
	}

	std::string getVIN(){
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

		this->send(this->map_commands.find("STATUS")->second);
		if (this->mapStatus["DTC_CNT"].compare("0")){
			this->send(this->map_commands.find("GET_DTC")->second);
		} else {
			std::cout << "No hay DTC disponibles" << std::endl;
		}
		return this->vecDTCs;
	}


	bool isValid(){
		return this->m_status;
	}
private:
      // Datos miembro de la clase "Obd"
   		//struct sockaddr_rc addr;
   		//int addr_len;
	std::vector<std::string> vecPIDs;
	std::vector<std::string> vecDTCs;
	std::string vin;
	std::string currentProtocol;
	std::string currentProtocolNumber;
	std::map<std::string, std::string> mapStatus;

	std::map<std::string, std::function<void()>> noDecodeFunctionAT;
	std::map<std::string, std::function<float(char *)>> decoderFunctionsFloat;
	std::map<std::string, std::function<struct OxigenoResponse(char *)>> decoderFunctionsStructOx;
	std::map<std::string, std::function<struct RelacionesResponse(char *)>> decoderFunctionsStructRel;
	std::map<std::string, std::function<std::vector<int>(char *)>> decoderFunctionsVectorInt;
	std::map<std::string, std::function<std::vector<std::string>(char *)>> decoderFunctionsVectorStr;
	std::map<std::string, std::function<std::string(char *)>> decoderFunctionsStr;
	std::map<std::string, std::function<std::map<std::string, std::string>(char *)>> decoderFunctionsMap;

	char dest[19] = { 0 };
	int m_cli_s;
	bool m_deviceFound = false;
	bool m_status = false;

	int epoll_fd;
	struct epoll_event ev;
};


#endif