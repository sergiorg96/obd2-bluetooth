/** 
* @file gpsclient.cpp
* @author Juan Manuel Vozmediano Torres
* @date 09/08/2019
* @brief Archivo que contiene la definición de la clase para la conexión con el servicio gpsd de obtención de coordenadas GPS.
*/

#include "gpsclient.hpp"
#include "loadcfg.hpp"

#define NOGPSDATA  "000000,010170,0000.000,N,00000.000,E" /**< Macro con la string en el caso de que no haya datos de GPS disponibles. */

GpsClient::GpsClient (std::string GpsPort, 
					  std::string validity):
  validity_(atoi(validity.c_str()))
{

  fprintf(stderr, "Init GpsClient %s %s\n", GpsPort.c_str(), validity.c_str());
  if ((s_= socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	shit ("socket");
  
  memset ((char *)&gpsin_, 0, sizeof(struct sockaddr_in));
  gpsin_.sin_family      = AF_INET;
  gpsin_.sin_addr.s_addr = inet_addr("127.0.0.1");
  gpsin_.sin_port        = htons(atoi(GpsPort.c_str()));
  
  memset ((char *)&iyo_, 0, sizeof(struct sockaddr_in));
  iyo_.sin_family      = AF_INET;
  iyo_.sin_addr.s_addr = INADDR_ANY;
  iyo_.sin_port        = 0;
  
  if (bind(s_, (struct sockaddr *)&iyo_, sizeof(iyo_)) == -1)
	shit("bind");
}


std::string GpsClient::getGPS()
{
  
  const int maxbuf = 512;
  char buffer [maxbuf];
  char msg[]    = "hhmmss,ddmmyy,llll.lll,N,yyyyy.yyy,E\n"; 
  struct timeval tv;
  int len = strlen(msg);

  memset(msg, 0, len);
  // Asks
  /*
  int cc =  sendto(s_,
                   buffer,
                   maxbuf,
                   0,
                   (struct sockaddr *)&gpsin_,
                   sizeof(gpsin_));
                   */
  sendto(s_, buffer, maxbuf, 0, (struct sockaddr *)&gpsin_, sizeof(gpsin_));
  
  fd_set rfds;
  tv.tv_sec  = 2;
  tv.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(s_, &rfds);
  int ret = select(s_+1, &rfds, NULL, NULL, &tv);
  
  if ((ret == 0)||(!FD_ISSET(s_, &rfds))) { /* timeout, return invalid data */
	    time_t lt;
	    //struct tm *p;
	    time(&lt);
	    strftime(msg, len, "%H%M%S,%d%m%y,,,,", localtime(&lt));
  }
  else {
	    struct sockaddr_in rv;
	    socklen_t          addrlen = sizeof(struct sockaddr_in);
	    (void) recvfrom(s_, msg, len, 0, (struct sockaddr *)&rv, &addrlen);
  }
  return std::string(msg);
}

