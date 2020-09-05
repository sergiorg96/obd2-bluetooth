/** 
* @file decoders.hpp
* @author Sergio Román González
* @date 05/09/2020
* @brief Archivo que contiene la declaración de las funciones de decodificación de las respuestas del dispositivo ELM327.
*/

#ifndef DECODERS_HPP
#define DECODERS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <vector>
#include <map>

#define PID_BITS 32 /**< Macro con el número de bits de respuesta para la solicitud de PIDs disponibles */
#define STATUS_BITS 8 /**< Macro con el número de bits de respuesta para las pruebas del PID STATUS */

/**
* @brief Estructura de datos para las respuesta de dos valores en PIDS relacionados con gases de escape.
*/

struct OxigenoResponse {
	float A; /**< Valor A en la formula de decodificación */
	float B; /**< Valor B en la formula de decodificación */
};

/**
* @brief Estructura de datos para las respuesta de cuatro valores en PIDS relacionados con gases de escape.
*/

struct RelacionesResponse {
	int A; /**< Valor A en la formula de decodificación */
	int B; /**< Valor B en la formula de decodificación */
	int C; /**< Valor C en la formula de decodificación */
	int D; /**< Valor D en la formula de decodificación */
};

/**
* @brief Función que no realiza decodificación para comandos AT.
*/

void noDecodeAT();

/**
* @brief Función de decodificación del PID STATUS
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Mapa string/string con el estado de los monitores de diagnóstico.
*/

std::map<std::string, std::string> decodeStatus(char *response);

/**
* @brief Función de decodificación de los PIDS disponibles en el vehículo.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Vector de enteros con los PIDS disponibles en el vehículo.
*/

std::vector<int> decodePIDS(char *response);

/**
* @brief Función de decodificación de los DTC activos en el vehículo.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Vector de strings con los DTC activos en el vehículo.
*/

std::vector<std::string> decodeDTCs(char *response);

/**
* @brief Función de conversión del primer byte del DTC en su valor correspondiente.
* @param dtc String con los bytes del DTC.
* @return String de DTC con el primer byte convertido.
*/

std::string convertDTCs(std::string dtc);

/**
* @brief Función de decodificación del Número de Identificación del Vehículo (VIN).
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return String con el Número de Identificación del Vehículo (VIN).
*/

std::string decodeVIN(char * response);

/**
* @brief Función de decodificación del protocolo de funcionamiento actual.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return String del protocolo de funcionamiento actual.
*/

std::string decodeDescribeProtocol(char * response);

/**
* @brief Función de decodificación de la posición EGR.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeCargaPosicionEGR(char *response);

/**
* @brief Función de decodificación de la temperatura.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeTempGeneral(char *response);

/**
* @brief Función de decodificación del ajuste de combustible EGR.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeAjusteCombustibleEGR(char *response);

/**
* @brief Función de decodificación de la presión del combustible.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodePresionCombustible(char *response);

/**
* @brief Función de decodificación de hexadecimal a decimal.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeHexToDec(char *response);

/**
* @brief Función de decodificación de las RPM del motor.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeRPM(char *response);

/**
* @brief Función de decodificación del avance del tiempo.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeAvanceTiempo(char *response);

/**
* @brief Función de decodificación de la tasa de flujo del aire (MAF).
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeVelocidadMAF(char *response);

/**
* @brief Función de decodificación de la presión del combustible del colector de vacío.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodePresionCombColector(char *response);

/**
* @brief Función de decodificación de la presión del medidor del tren de combustible.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodePresionMedidorCombustible(char *response);

/**
* @brief Función de decodificación de la presión de vapor del sistema evaporativo .
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodePresionVapor(char *response);

/**
* @brief Función de decodificación de la temperatura del catalizador.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeTempCatalizador(char *response);

/**
* @brief Función de decodificación del voltaje del módulo de control.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeVoltajeControl(char *response);

/**
* @brief Función de decodificación de la relación equivaliente comandada de combustible - aire.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Float del valor de la respuesta decodificada.
*/

float decodeRelacionCombAireBasica(char *response);

/**
* @brief Función de decodificación de los sensores de oxígeno.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Estructura OxigenoResponse con el valor A y B correspondiente al comando solicitado.
*/

struct OxigenoResponse decodeSensorOxigeno(char *response);

/**
* @brief Función de decodificación de los sensores de oxígeno y la relación de combustible.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Estructura OxigenoResponse con el valor A y B correspondiente al comando solicitado.
*/

struct OxigenoResponse decodeRelacionCombAire(char *response);

/**
* @brief Función de decodificación de los sensores de oxígeno y la relación de combustible actual.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Estructura OxigenoResponse con el valor A y B correspondiente al comando solicitado.
*/

struct OxigenoResponse decodeRelacionCombAireActual(char *response);

/**
* @brief Función de decodificación del alor máximo de la relación de equivalencia de combustible - aire,
* voltaje del sensor de oxígenos, corriente del sensor de oxígenos y presión absoluta
* del colector de entrada.
* @param response Cadena de caracteres con los bytes útiles de la respuesta del dispositivo ELM327.
* @return Estructura OxigenoResponse con el valor A y B correspondiente al comando solicitado.
*/

struct RelacionesResponse decodeRelaciones(char *response);


#endif