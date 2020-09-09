/** 
* @file gpsclient.hpp
* @author Juan Manuel Vozmediano Torres
* @date 09/08/2019
* @brief Archivo que contiene la declaración de la clase para la conexión con el servicio gpsd de obtención de coordenadas GPS.
*/

#ifndef GPSCLIENT_HPP
#define GPSCLIENT_HPP

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

/** 
* @brief Clase que representa la conexión con el servicio gpsd para obtener las coordenadas GPS.
*
* Clase utilizada para la obtención de coordenadas GPS.
*/

class GpsClient {
  
 private:
  int validity_; /**< Entero con el tiempo máximo de espera en segundos en la recepción del dato GPS. */
  struct sockaddr_in gpsin_; /**< Estructura con la información para el envío del datagrama de solicitud. */
  struct sockaddr_in iyo_; /**< Estructura para el bind del socket. */
  int s_; /**< Entero que identifica el socket de conexión. */

 public:

  /** 
  * @brief Constructor de la clase GpsClient.
  * @param GpsPort String del puerto UDP de conexión con el servicio gpsd.
  * @param validity String con el tiempo máximo de espera en segundos en la recepción del dato GPS.
  * @return Devuelve una instancia de la clase GpsClient.
  */ 

  GpsClient (std::string GpsPort, std::string validity); 

  /** 
  * @brief Método que obtiene una string con las coordenadas GPS.
  * @return String con las coordenadas GPS.
  */ 

  std::string getGPS ();
};

#endif
