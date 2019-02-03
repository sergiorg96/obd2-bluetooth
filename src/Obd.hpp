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

#include <unistd.h>

#include "Commands.hpp"

using json = nlohmann::json;

class Obd {
public:
	std::vector<Commands> m_commands;
      // Constructor
	Obd(){
		this->connectBluetooth();
		//if(this->m_status)
			this->readFileData();
	}
      // Funciones miembro de la clase "Obd"
      //void getDato();
      //void setDato(int dato);
	void connectBluetooth(){
		try{
			struct sockaddr_rc addr;
			int statusConnection;
			char dest[18] = "00:1D:A5:00:0D:AC";


			this->m_cli_s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
			if (this->m_cli_s < 0) {
				throw std::string("error opening BT/RFCOMM socket");
			}
			//Faltaría sustituir automaticamente la MAC
			addr.rc_family = AF_BLUETOOTH;
			str2ba(dest, &addr.rc_bdaddr );
			addr.rc_channel = (uint8_t) 1;

			printf("connecting to %s (channel %d)\n", dest, addr.rc_channel);

			// connect to server, throw socket s (m_cli_s -> descriptor)
			statusConnection = connect(this->m_cli_s, (struct sockaddr *)&addr, sizeof(addr));

			if (statusConnection) {
				close(this->m_cli_s);
				throw std::string("unable to connect");
			}

			printf("connected\n");

		} catch(std::string e) {
			std::cerr << e << std::endl;
			this->m_status = false;
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
	int m_cli_s;
	bool m_status = true;
};


#endif