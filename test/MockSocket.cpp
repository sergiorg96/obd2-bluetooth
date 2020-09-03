#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
//Para mkfifo
#include <sys/types.h>
#include <sys/stat.h>
#include "../src/debug.hpp"

std::string findDevPTS(){
    system("ls /dev/pts | tail -2 | head -1 > tmpPTSfile.txt");

    int ultPts;
    int tempVar;
    std::ifstream input_file("tmpPTSfile.txt");
    while ( input_file >> tempVar )
    {
        ultPts = tempVar;
    }

    std::string devFile = "/dev/pts/" + std::to_string(ultPts);
    remove("tmpPTSfile.txt");

    return devFile;
}

int mock_socket(int domain, int type, int protocol){

	std::string devFile = findDevPTS();

    int filedesc = open(devFile.c_str(), O_RDWR);
    if (filedesc < 0) {
    	debugError("Error al abrir socket %d.", filedesc);
    }

    return filedesc;
}

int hci_get_route( bdaddr_t *bdaddr ){
	int value = 0;
	return value;
}

int hci_open_dev( int dev_id ){
	int filedesc = open("/dev/null", O_RDONLY);	
	if(filedesc < 0)
        filedesc = -1;
    return filedesc;
}

int hci_inquiry(int dev_id, int len, int max_rsp, const uint8_t *lap, inquiry_info **ii, long flags){
	int value = 1;
	return value;
}


int hci_read_remote_name(int sock, const bdaddr_t *ba, int len, char *name, int timeout){
	int value = 1;
	strcpy(name, "OBDII");
	return value;	
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	int value = 0;
	return value;
}

void writeSocket(){
	
}
