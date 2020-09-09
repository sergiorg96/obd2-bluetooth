/** 
* @file loadcfg.cpp
* @author Juan Manuel Vozmediano Torres
* @date 09/04/2019
* @brief Archivo que contiene la definición de las funciones para la lectura de un fichero de configuración del tipo clave=valor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>
#include <ifaddrs.h>
#include <ctype.h>
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <netpacket/packet.h>
#include "loadcfg.hpp"

void shit (const char* mens)
{
std::cerr << "ABORTING: " << mens << " - " << errno << "\n";
perror("Error is ");
exit(1);
}

void loadCfg (const char* filename, cfgType* pcfg)
{
   std::ifstream cFile (filename);
   if (cFile.is_open()){
     std::string line;
     while(getline(cFile, line)){
       line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
       if(line[0] == '#' || line.empty())
         continue;
       int delimiterPos = line.find("=");
       std::string name = line.substr(0, delimiterPos).c_str();
       std::string value = line.substr(delimiterPos + 1).c_str();
       (*pcfg)[name] = value;
     }
   }
   else {
     shit("Couldn't open config file for reading.\n");
   }
}


std::string getmac (const char* name)
{
   int i;
   struct ifaddrs *addrs,*tmp;
   std::stringstream macaddress;
   char mymac[25];
   getifaddrs(&addrs);
   tmp = addrs;
   memset (mymac, 0, 25);
   while (tmp) {
     if (!strcmp(name, tmp->ifa_name)){
          struct sockaddr_ll *s = (struct sockaddr_ll*)tmp->ifa_addr;
          for (i=0; i <s->sll_halen; i++){
         sprintf(mymac, "%s%02x%c",
                                mymac,
                                (s->sll_addr[i]),
                                (i+1!=s->sll_halen)?':':0);
          }
          macaddress << mymac;
          return macaddress.str();
        }
     tmp = tmp->ifa_next;
   }
   freeifaddrs(addrs);
   return std::string("ff:ff:ff:ff:ff:ff");
}