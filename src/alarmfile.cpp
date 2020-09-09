/** 
* @file alarmfile.cpp
* @author Juan Manuel Vozmediano Torres
* @date 09/04/2019
* @brief Archivo que contiene la definición de la clase para la configuración y envío de un mensaje a un servidor remoto.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include "alarmfile.hpp"

void AlarmFile::shit (const char *mens)
{
  fprintf(stderr, "%s - %d\n", mens, errno);
  perror("Error is ");
}

AlarmFile::AlarmFile(std::string AlarmHost,
					 std::string AlarmPort,
					 std::string AlarmFilename,
					 std::string LastAlarmFilename):
  alarmHost_(AlarmHost),
  alarmPort_(atoi(AlarmPort.c_str())),
  alarmFilename_(AlarmFilename),
  lastAlarmFilename_(LastAlarmFilename)
{
  if ((s_ = socket (AF_INET, SOCK_DGRAM, 0)) < 0) shit ("socket");

  memset ((char *)&iTu_, 0, sizeof(struct sockaddr_in));
  iTu_.sin_family      = AF_INET;  
  iTu_.sin_addr.s_addr = inet_addr(alarmHost_.c_str()); 
  iTu_.sin_port        = htons(alarmPort_);
}


bool AlarmFile::sendAlarm(std::string msg)
{
  if ( "" != msg ){
    int cc =  sendto(s_, msg.c_str(), strlen(msg.c_str()), 0,(struct sockaddr *)&iTu_, sizeof(iTu_));

    if (cc < 0){
      perror("Error is ");
      fprintf(stderr, "Value of errno: %d\n", errno);
    }

    fprintf(stderr, "Alarma enviada (%d): %s a %s:%d\n", cc, msg.c_str(), inet_ntoa(iTu_.sin_addr), (int) ntohs(iTu_.sin_port));

    return true;
  }
  return false;
}

