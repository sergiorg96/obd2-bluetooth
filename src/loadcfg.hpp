/** 
* @file loadcfg.hpp
* @author Juan Manuel Vozmediano Torres
* @date 09/04/2019
* @brief Archivo que contiene la declaración de las funciones para la lectura de un fichero de configuración del tipo clave=valor.
*/

#ifndef LOADCFG_HPP
#define LOADCFG_HPP

/** 
* @brief Definición del tipo cfgType para referenciar los parámetros de configuración y sus valores.
*/

typedef  std::map <std::string, std::string> cfgType;

/**
* @brief Función para indicar error en el código y terminar la ejecución.
* @param mens Cadena de caracteres para mostrar en el error producido.
*/

void shit (const char* mens);

/**
* @brief Función cargar la configuración y almacenarla para su utilización.
* @param filename Cadena de caracteres del archivo de configuración a leer.
* @param pcfg Variable de tipo puntero a cfgType para referenciar la variable donde se almacenará la configuración.
*/

void loadCfg (const char* filename, cfgType* pcfg);

/**
* @brief Función que obtiene la MAC de una interfaz de red indicada.
* @param name Cadena de caracteres indicando el nombre de la interfaz de red de la que obtener su MAC.
* @return String de la MAC de la interfaz de red indicada.
*/

std::string getmac (const char* name);

#endif