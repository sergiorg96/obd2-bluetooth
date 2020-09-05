/** 
* @file alarmfile.hpp
* @author Juan Manuel Vozmediano Torres
* @date 09/04/2019
* @brief Archivo que contiene la declaración de la clase para la configuración y envío de un mensaje a un servidor remoto.
*/


#ifndef ALARMFILE_HPP
#define ALARMFILE_HPP

#include <string>
#include <netinet/in.h>

/** 
* @brief Clase que representa la conexión con el servidor remoto para el envío de un mensaje (alarma).
*
* Clase utilizada para el envío de datos del vehículo al servidor remoto.
*/

class AlarmFile{
public:

  /** 
  * @brief Constructor de la clase AlarmFile.
  * @param AlarmHost String con la dirección IP del servidor remoto.
  * @param AlarmPort String con el puerto de conexión del servidor remoto.
  * @param AlarmFilename String con el nombre del archivo de almacenamiento de la alarma.
  * @param LastAlarmFilename String con el nombre del último archivo de almacenamiento de la alarma.
  * @return Devuelve una instancia de la clase AlarmFile.
  */ 

  AlarmFile(std::string AlarmHost,
           std::string AlarmPort,
           std::string AlarmFilename,
           std::string LastAlarmFilename);

  /**
  * @brief Método que obtiene la geolocalización del vehículo.
  * @param serialPort String con la ruta al dispositivo GPS.
  * @return String con la geolocalización del dispositivo GPS.
  */

  std::string getGeoPos(std::string serialPort);

  /**
  * @brief Método para mostrar si tiene disponible el GPS.
  * @return Booleano, true si está instalado y false en caso contrario.
  */

  bool hasGps();

    /**
  * @brief Método para indicar que está disponible el GPS.
  * @param installed Booleano que indica si está instalado el GPS (true).
  */

  void Gps(bool installed);

  /**
  * @brief Método para enviar el mensaje/alarma al servidor remoto.
  * @param msg String con el mensaje a enviar al servidor remoto.
  * @return Booleano, true si el mensaje fue enviado correctamente y false en caso contrario.
  */

  bool sendAlarm(std::string msg);
private:

  /**
  * @brief Método que indica el error producido en cierto código.
  * @param mens Cadena de caracteres con el mensaje de error a indicar.
  */

  void shit (const char *mens);
  static bool gps_; /**< Booleano que indica si está instalado el GPS. */
  std::string alarmHost_; /**< String con la dirección del servido remoto. */
  int alarmPort_; /**< Entero con el puerto de conexión del servidor remoto. */
  std::string alarmFilename_; /**< String con el nombre del archivo de almacenamiento de la alarma. */
  std::string lastAlarmFilename_; /**< String con el nombre del último archivo de almacenamiento de la alarma. */
  int s_; /**< Entero con el socket de conexión con el servidor remoto. */
  struct sockaddr_in iTu_; /**< Estructura sockaddr_in para el envío del mensaje a través del socket. */
};

#endif