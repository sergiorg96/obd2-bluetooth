#ifndef ALARMFILE_HPP
#define ALARMFILE_HPP

#include <string>
#include <netinet/in.h>

class AlarmFile{
public:
  AlarmFile(std::string AlarmHost,
           std::string AlarmPort,
           std::string AlarmFilename,
           std::string LastAlarmFilename);
  std::string getGeoPos(std::string serialPort);
  bool hasGps();
  void Gps(bool installed);
  bool sendAlarm(std::string msg);
private:
  void shit (const char *mens);
  static bool gps_;
  std::string alarmHost_;
  int alarmPort_;
  std::string alarmFilename_;
  std::string lastAlarmFilename_;
  int s_;
  struct sockaddr_in iTu_;
};

#endif