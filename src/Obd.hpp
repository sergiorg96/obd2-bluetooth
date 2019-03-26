#ifndef OBD_HPP
#define OBD_HPP

#include <iostream>
#include <fstream>

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

#include "Commands.hpp"

using json = nlohmann::json;

class Obd {
public:
	std::vector<Commands> m_commands;
      // Constructor
	Obd(char *deviceName){
		this->discoverDeviceAddress(deviceName, this->dest);
		if(this->m_deviceFound)
			this->connectBluetooth(dest);
		else
			printf("Device %s not found.\n", deviceName);
		//if(this->m_status)
			this->readFileData();
	}
      // Funciones miembro de la clase "Obd"
      //void getDato();
      //void setDato(int dato);
	void connectBluetooth(char *dest){
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
			str2ba(dest, &addr.rc_bdaddr );
			addr.rc_channel = (uint8_t) 2;

			printf("connecting to %s (channel %d)\n", dest, addr.rc_channel);

			// connect to server, throw socket s (m_cli_s -> descriptor)
			statusConnection = connect(this->m_cli_s, (struct sockaddr *)&addr, sizeof(addr));

			if (statusConnection) {
				close(this->m_cli_s);
				throw std::string("unable to connect");
			}

			printf("connected\n");
			this->m_status = true;

		} catch(std::string e) {
			std::cerr << e << std::endl;
		}
	}

	void readFileData(){
		std::ifstream ifs("data/PIDS.json");
		auto j = json::parse(ifs);

		for (int i = 0; i < (int)j.size(); ++i)
		{
			this->m_commands.push_back(Commands(j[i]));
		}
	}

	void discoverDeviceAddress(char * deviceName, char *deviceAddress){
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
			}
		}

		free( ii );
		close( sock );
	}


	void disconnectBluetooth(){
		close(this->m_cli_s);
	}

	
	
	bool isValid(){
		return this->m_status;
	}
private:
      // Datos miembro de la clase "Obd"
   		//struct sockaddr_rc addr;
   		//int addr_len;
	char dest[19] = { 0 };
	int m_cli_s;
	bool m_deviceFound = false;
	bool m_status = false;
};


#endif