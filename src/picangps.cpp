#include "picangps.hpp"

int openGPS (std::string tty) { 
  int fd;
  struct termios options;
  
  fd=open(tty.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
  int result = flock(fd, LOCK_EX);
  if(result==0){
    fcntl(fd, F_SETFL, 0);
    if( 0 != tcgetattr(fd, &options))
     return -1;

   options.c_cflag &= ~PARENB;
   options.c_cflag &= ~CSTOPB;
   options.c_cflag &= ~CSIZE;
   options.c_cflag |= CS8;

   options.c_cflag |= (CLOCAL | CREAD);

  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* raw input */

   options.c_oflag &= ~OPOST;

   options.c_cc[VMIN]  = 0;
   options.c_cc[VTIME] = 0;

   cfsetispeed(&options, B9600);
   cfsetospeed(&options, B9600);
   if (0 != tcsetattr(fd, TCSANOW, &options))
     return -1;
   return fd;
 }
}

bool readPicanGPS(int fd, char* buffer, int maxsiz, int seconds)
{
  fd_set rfds;
  int ret;
  struct timeval tv;
  char* bufptr;
  int  maxchars = 800;
  int  nbytes;       /* Number of bytes read */
  const char* tag = "$GPRMC,";
  char* startline;
  char* endline;
  
  if (fd < 0) 
	return false;
  
  bufptr = buffer;
  for(;maxchars>0;){
	//	fprintf(stderr, "->Select GPS\n");
	tv.tv_sec = seconds;
	tv.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	ret = select(fd+1, &rfds, NULL, NULL, &tv);

	//	fprintf(stderr, "ret is %d\n",ret);
	if (ret == 0) { /* timeout, return invalid data */
	  return false;
	}
	if (FD_ISSET(fd, &rfds)){
	  //	  nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1);
	  nbytes = read(fd, bufptr, buffer - bufptr + maxsiz - 1);
	  maxchars -= nbytes;
	  bufptr += nbytes;
	  *bufptr = '\0';
	  //	  fprintf(stderr, "Reading %d bytes from fd \"%s\"\n", nbytes, buffer);

	  if (strlen(buffer)>5*strlen(tag)){
		startline = strstr(buffer, tag);
		if ( NULL == startline ){
		  bufptr = buffer;
		  continue;
		}
		// else got line header
		endline = strstr(startline, "\n");
		if ( NULL == endline ){
		  // gather more data
		  continue;
		}
		else {
		  //move to start of buffer and return
		  char* pc=startline;
		  bufptr = buffer;
		  for(;pc<=endline;pc++, bufptr++)
			*bufptr = *pc;
		  *bufptr='\0';
		  return true;
		}
	  }
	  else {
		;
		//fprintf(stderr, "get more data\n");
	  }
	}
  }
  //fprintf(stderr, "too many tries\n");
  return false;
}

/**
 * $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
 * 
 * Where:
 *      RMC          Recommended Minimum sentence C
 *      123519       Fix taken at 12:35:19 UTC
 *      A            Status A=active or V=Void.
 *      4807.038,N   Latitude 48 deg 07.038' N
 *      01131.000,E  Longitude 11 deg 31.000' E
 *      022.4        Speed over the ground in knots
 *      084.4        Track angle in degrees True
 *      230394       Date - 23rd of March 1994
 *      003.1,W      Magnetic Variation
 *      *6A          The checksum data, always begins with *
 */


/* field, long */



/**
 * Returns pointer to the field-th field in s, fields separated by sep. 
 */
char* explore (int field, char sep, char* s)
{
  for (; field; field--){
	for (; *s && *s != sep; s++);
	if(*s == sep)
	  s++;
	else
	  return 0;
  }
  return s;
}

std::string PicanGetGPS (std::string serialPort)
{
  const int maxbuf = 2560;
  char buffer [maxbuf];
  char msg[]    = "hhmmss,ddmmyy,llll.lll,N,yyyyy.yyy,E"; 

  memset(buffer, 0, maxbuf);
  std::string retval(",,,,,");

  int      fd = openGPS(serialPort); 
  bool  valid = readPicanGPS(fd, buffer, maxbuf, 3);
  close(fd);
  if (!valid)
	return retval;

  char* pc;
  pc = explore(2, ',', buffer);
  if ('A' != *pc)	
	return retval;
  	  
  strncpy(&msg[0],   explore(1, ',', buffer), 6);
  strncpy(&msg[7],   explore(9, ',', buffer), 6);
  strncpy(&msg[14],  explore(3, ',', buffer), 8);
  strncpy(&msg[23],  explore(4, ',', buffer), 1);
  strncpy(&msg[25],  explore(5, ',', buffer), 9);
  strncpy(&msg[35],  explore(6, ',', buffer), 1);

  return std::string(msg);
}

