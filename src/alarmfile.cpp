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

#include "picangps.hpp"

bool AlarmFile::gps_ = false;

std::string AlarmFile::getGeoPos(std::string serialPort)
{
  return PicanGetGPS(serialPort);
}

bool AlarmFile::hasGps()
{
  return AlarmFile::gps_;
}

void AlarmFile::Gps(bool installed)
{
  AlarmFile::gps_ = installed;
}

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
    int cc =  sendto(s_,
                   msg.c_str(),
                   strlen(msg.c_str()),
                   0,
                   (struct sockaddr *)&iTu_,
                   sizeof(iTu_));
  
    fprintf(stderr, "Alarma enviada (%d): %s a %s:%d\n", cc, msg.c_str(), inet_ntoa(iTu_.sin_addr), (int) ntohs(iTu_.sin_port));

    return true;
  }
  return false;
}

