/** 
* @file debug.hpp
* @author Sergio Román González
* @date 05/09/2020
* @brief Archivo que contiene las funciones de debug en la salida estándar y de error del sistema.
*/

#include <stdio.h>

#ifndef DEBUG_HPP
#define DEBUG_HPP
#ifdef DEBUG
	/**
	* @brief Macro de función para el debug del nivel de Log.
	*/
 	#define debugLog(info, args...) \
		fprintf (stderr, "[%s %s][LOG][%s][%s][Line %i] ", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__); \
		fprintf (stderr, info "\n", ##args);
	/**
	* @brief Macro de función para el debug del nivel de Error.
	*/
	#define debugError(info, args...) \
        fprintf (stderr, "[%s %s][ERROR][%s][%s][Line %i] ", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__); \
        fprintf (stderr, info "\n", ##args);
#else
    /**
	* @brief Macro de función vacía para el debug del nivel de Log.
	*/
	#define debugLog(format, args...) do{} while(0);
    /**
	* @brief Macro de función vacía para el debug del nivel de Error.
	*/
	#define debugError(format, args...) do{} while(0);
#endif
#endif