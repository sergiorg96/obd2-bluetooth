#include <stdio.h>
#include <fcntl.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
//Para mkfifo
#include <sys/types.h>
#include <sys/stat.h>
#include "../src/debug.hpp"

int mock_socket(int domain, int type, int protocol){
	// Crear socket con FIFO

	//const char * myfifo = "fifo_file"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    //int err = mkfifo(myfifo, 0666);
  
    //if (err == -1)
    //	return err;

	printf("He llegado a mock_socket\n");

    //int filedesc = open(myfifo, O_RDWR);
    int filedesc = open("/dev/pts/3", O_RDWR);
    if (filedesc < 0) {
    	debugError("Error al abrir socket %d.", filedesc);
    }

    //printf("calling mock socket\n");
    return filedesc;
}

int hci_get_route( bdaddr_t *bdaddr ){
	int value = 0;
	//printf("calling mock hci_get_route\n");
	return value;
}

int hci_open_dev( int dev_id ){
	int filedesc = open("/dev/null", O_RDONLY);	
	if(filedesc < 0)
        filedesc = -1;
    //printf("calling mock hci_open_dev\n");
    return filedesc;
}

int hci_inquiry(int dev_id, int len, int max_rsp, const uint8_t *lap, inquiry_info **ii, long flags){
	int value = 1;
	//printf("calling mock hci_inquiry\n");
	return value;
}


int hci_read_remote_name(int sock, const bdaddr_t *ba, int len, char *name, int timeout){
	int value = 1;
	//printf("calling mock hci_read_remote_name\n");
	strcpy(name, "OBDII");
	return value;	
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	int value = 0;
	//printf("calling mock connect\n");
	return value;
}

void writeSocket(){
	
}
