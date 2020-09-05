/** 
* @file Commands.hpp
* @author Sergio Román González
* @date 05/09/2020
* @brief Archivo que contiene la clase con la definición de la estructura de los comandos AT y OBD.
*/

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <any>

#include "external/json.hpp"
#include "decoders.hpp"

/**
* @brief Utilización de la librería externa nlohmann::json a través del tipo definido json.
*/

using json = nlohmann::json;

/** 
* @brief Clase que representa los comandos AT y PIDS que se necesitan en el intercambio de mensajes con el dispositivo ELM327.
*
* Clase utilizada por la clase Obd con la información relativa a los comandos OBD.
*/

class Commands {
public:

	/** 
 	* @brief Constructor de la clase Commands.
 	* @param data Tipo de datos json con la lista de comandos AT y OBD genéricos.
 	* @return Devuelve una instancia de la clase Commands.
 	*/ 

	Commands(json data) 
	:   m_name(data["name"]),
		m_description(data["description"]),
		m_cmd(data["cmd"]),
		m_bytes_response(data["bytes_response"]),
		m_decoder(data["decoder"]),
		m_min_unit(data["min_unit"]),
		m_max_unit(data["max_unit"]),
		m_units(data["units"]),
		m_type_data(data["type_data"])
	{}
    
	/**
	* @brief Método que obtiene el nombre del comando.
	* @return String con el nombre del comando.
	*/

	std::string getName(){ return this->m_name; }

	/**
	* @brief Método que obtiene la descripción del comando.
	* @return String con la descripción del comando.
	*/

	std::string getDescription(){ return this->m_description; }

	/**
	* @brief Método que obtiene el comando en hexadecimal que se envía al dispositivo ELM327.
	* @return String con el comando en hexadecimal que se envía al dispositivo ELM327.
	*/

	std::string getCMD(){ return this->m_cmd; }

	/**
	* @brief Método que obtiene el número de bytes de la respuesta del comando a enviar.
	* @return Entero con el número de bytes de la respuesta del comando a enviar.
	*/

	int getBytesResponse(){ return this->m_bytes_response; }

	/**
	* @brief Método que obtiene la función de decodificación que se debe ejecutar en la respuesta.
	* @return String del decodificador que se debe ejecutar en la respuesta.
	*/

	std::string getDecoder(){ return this->m_decoder; }

	/**
	* @brief Método que obtiene el valor mínimo que puede tener la respuesta al comando.
	* @return Flotante con el valor mínimo que puede tener la respuesta al comando.
	*/

	float getMIN(){ return this->m_min_unit; }

	/**
	* @brief Método que obtiene el valor máximo que puede tener la respuesta al comando.
	* @return Flotante con el valor máximo que puede tener la respuesta al comando.
	*/

	float getMAX(){ return this->m_max_unit; }

	/**
	* @brief Método que obtiene en qué unidades se mide la respuesta del comando.
	* @return String de la unidad de medida de la respuesta del comando.
	*/

	std::string getUnits(){ return this->m_units; }

	/**
	* @brief Método que obtiene el tipo de dato que se obtiene en la respuesta del comando.
	* @return String del tipo de dato de la respuesta del comando.
	*
	* El tipo de dato se utiliza para filtrar entre los tipos de decodificadores de respuesta.
	* Sus valores pueden ser: string, float, vector<int>, vector<string>, map<string, string>,
	* struct OxigenoResponse y struct RelacionesResponse.
	*/

	std::string getTypeData(){ return this->m_type_data; }

	/**
	* @brief Método que obtiene el valor decodificado de la respuesta del comando.
	* @return El tipo de dato correspondiente con el comando y el valor decodificado de la respuesta.
	*/

	std::any getResValue(){ return this->m_resValue; }

	/**
	* @brief Método que obtiene información del comando y el valor de la respuesta decodificado en formato JSON.
	* @return Tipo json definido con el valor de la respuesta decodificado del comando, su nombre, descripción y unidades.
	*
	* Función desarrollada con el fin de facilitar el envío de información en formato JSON a un servidor remoto
	* o para el almacenamiento local.
	*/

	json getJson(){
		json data;

		try{
			if (getTypeData() == "int"){
				auto resValue = std::any_cast<int>(this->m_resValue);
				data["value"] = std::to_string(resValue);
			}
			else if (getTypeData() == "float"){
				auto resValue = std::any_cast<float>(this->m_resValue);
				data["value"] = std::to_string(resValue);
			}
			else if (getTypeData() == "string"){
				auto resValue = std::any_cast<std::string>(this->m_resValue);
				data["value"] = resValue;
			}
			else if (getTypeData() == "vectorStr"){
				auto resValue = std::any_cast<std::vector<std::string>>(this->m_resValue);
				data["value"] = resValue;
			}
			else if (getTypeData() == "vectorInt"){
				auto resValue = std::any_cast<std::vector<int>>(this->m_resValue);
				data["value"] = resValue;
			}
			else if (getTypeData() == "map"){
				auto resValue = std::any_cast<std::map<std::string, std::string>>(this->m_resValue);
				data["value"] = resValue;
			}
			else if (getTypeData() == "OxigenoResponse"){
				auto resValue = std::any_cast<struct OxigenoResponse>(this->m_resValue);
				std::map<std::string, float> mapResValue;
				mapResValue["A"] = resValue.A;
				mapResValue["B"] = resValue.B;
				data["value"] = mapResValue;
			}
			else if (getTypeData() == "RelacionesResponse"){
				auto resValue = std::any_cast<struct RelacionesResponse>(this->m_resValue);
				std::map<std::string, int> mapResValue;
				mapResValue["A"] = resValue.A;
				mapResValue["B"] = resValue.B;
				mapResValue["C"] = resValue.C;
				mapResValue["D"] = resValue.D;
				data["value"] = mapResValue;
			}

		} catch(const std::bad_any_cast& e) {
			std::cerr << e.what() << std::endl;
		}
		data["name"] = this->m_name;
		data["description"] = this->m_description;
		data["units"] = this->m_units;
		

		return data;
	}

	/**
	* @brief Método para obtener el comando de respuesta al PID solicitado.
	* @return String con la cadena de respuesta sustituyendo el 0 por el 4 en el mensaje OBD.
	*
	* Se utiliza para identificar los bytes útiles de la respuesta que se encuentran tras esta cadena.
	*/

	std::string getCMDResponse() {
		std::string CMDResponse;
		CMDResponse = this->m_cmd;
		CMDResponse.replace(0, 1, "4");
		return CMDResponse;
	}

	/**
	* @brief Método para asignar un nombre a un comando.
	* @param name String con el nombre a asignar al comando.
	*/

	void setName(std::string name) { this->m_name = name; }

	/**
	* @brief Método para asignar una descripción a un comando.
	* @param description String con la descripción a asignar al comando.
	*/

	void setDescription(std::string description) { this->m_description = description; }

	/**
	* @brief Método para asignar el comando en hexadecimal a enviar al dispositivo ELM327.
	* @param cmd String con el comando en hexadecimal a enviar al dispositivo ELM327.
	*/

	void setCMD(std::string cmd) { this->m_cmd = cmd; }

	/**
	* @brief Método para asignar el número de bytes de respuesta a un comando.
	* @param bytes_response Entero con el número de bytes de respuesta a asignar al comando.
	*/

	void setBytesResponse(int bytes_response) { this->m_bytes_response = bytes_response; }

	/**
	* @brief Método para asignar un decodificador a un comando.
	* @param decoder String con el nombre del decodificador que utiliza el comando.
	*/

	void setDecoder(std::string decoder) { this->m_decoder = decoder; }

	/**
	* @brief Método para asignar el valor mínimo de la respuesta a un comando.
	* @param min_unit Flotante con el valor mínimo de la respuesta a un comando.
	*/

	void setMIN(float min_unit) { this->m_min_unit = min_unit; }

	/**
	* @brief Método para asignar el valor máximo de la respuesta a un comando.
	* @param max_unit Flotante con el valor máximo de la respuesta a un comando.
	*/

	void setMAX(float max_unit) { this->m_max_unit = max_unit; }

	/**
	* @brief Método para asignar las unidades de medida de la respuesta a un comando.
	* @param units String con las unidades de medida de la respuesta del comando.
	*/

	void setUnits(std::string units) { this->m_units = units; }

	/**
	* @brief Método para asignar el tipo de dato que se debe de obtener en la respuesta del comando.
	* @param type_data String con el tipo de dato que se debe de obtener en la respuesta del comando.
	*/

	void setTypeData(std::string type_data) { this->m_type_data = type_data; }

	/**
	* @brief Método para asignar el valor decodificado de la respuesta al comando.
	* @param resValue Tipo de dato dependiente del tipo de dato del comando con el valor de la respuesta decodificada.
	*
	* Función utilizada para el almacenamiento en memoria del valor solicitado con un comando.
	*/

	void setResValue(auto resValue) { this->m_resValue = resValue; }
private:
    // Atributos privados de la clase "Commands"
	std::string m_name; /**< Atributo de tipo string con el nombre del comando. */
	std::string m_description; /**< Atributo de tipo string con la descripción del comando. */
	std::string m_cmd; /**< Atributo de tipo string con el comando en hexadecimal que se envía al dispositivo ELM327. */
	int m_bytes_response; /**< Atributo de tipo entero con el número de bytes de la respuesta del comando a enviar. */
	std::string m_decoder; /**< Atributo de tipo string del decodificador que se debe ejecutar en la respuesta.  */
	float m_min_unit; /**< Atributo de tipo flotante con el valor mínimo que puede tener la respuesta al comando. */
	float m_max_unit; /**< Atributo de tipo flotante con el valor máximo que puede tener la respuesta al comando. */
	std::string m_units; /**< Atributo de tipo string de la unidad de medida de la respuesta del comando. */
	std::string m_type_data; /**< Atributo de tipo string del tipo de dato de la respuesta del comando. */
	std::any m_resValue; /**< Atributo del tipo de dato correspondiente con el comando y el valor decodificado de la respuesta. */
};


#endif