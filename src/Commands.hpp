#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <any>

#include "external/json.hpp"
#include "decoders.hpp"

using json = nlohmann::json;

class Commands {
public:
      // Constructor
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
      // Funciones miembro de la clase "Commands"
	std::string getName(){ return this->m_name; }
	std::string getDescription(){ return this->m_description; }
	std::string getCMD(){ return this->m_cmd; }
	int getBytesResponse(){ return this->m_bytes_response; }
	std::string getDecoder(){ return this->m_decoder; }
	float getMIN(){ return this->m_min_unit; }
	float getMAX(){ return this->m_max_unit; }
	std::string getUnits(){ return this->m_units; }
	std::string getTypeData(){ return this->m_type_data; }
	std::any getResValue(){ return this->m_resValue; }

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

	//Obtiene la cadena de respuesta sustituyendo el 0 por el 4 en el mensaje OBD
	std::string getCMDResponse() {
		std::string CMDResponse;
		CMDResponse = this->m_cmd;
		CMDResponse.replace(0, 1, "4");
		return CMDResponse;
	}

	void setName(std::string name) { this->m_name = name; }
	void setDescription(std::string description) { this->m_description = description; }
	void setCMD(std::string cmd) { this->m_cmd = cmd; }
	void setBytesResponse(int bytes_response) { this->m_bytes_response = bytes_response; }
	void setDecoder(std::string decoder) { this->m_decoder = decoder; }
	void setMIN(float min_unit) { this->m_min_unit = min_unit; }
	void setMAX(float max_unit) { this->m_max_unit = max_unit; }
	void setUnits(std::string units) { this->m_units = units; }
	void setTypeData(std::string type_data) { this->m_type_data = type_data; }
	void setResValue(auto resValue) { this->m_resValue = resValue; }
private:
    // Atributos privados de la clase "Commands"
	std::string m_name;
	std::string m_description;
	std::string m_cmd;
	int m_bytes_response;
	std::string m_decoder;
	float m_min_unit;
	float m_max_unit;
	std::string m_units;
	std::string m_type_data;
	std::any m_resValue;
};


#endif