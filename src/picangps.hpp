#ifndef PICANGPS_HPP
#define PICANGPS_HPP

#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/file.h>


int openGPS (std::string tty);
bool readPicanGPS(int fd, char* buffer, int maxsiz, int seconds);
char* explore (int field, char sep, char* s);
std::string PicanGetGPS (std::string serialPort);

#endif