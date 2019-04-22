#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>

#include "json.hpp"

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

	json getJson(){
		/*
		json command = {
			{"name", this->m_name},
			{"description", this->m_description},
			{"cmd", this->m_cmd},
			{"bytes_response", this->m_bytes_response},
			{"m_decoder", this->m_decoder}
		};*/
		json command;
		command["name"] = this->m_name;
		command["description"] = this->m_description;
		command["cmd"] = this->m_cmd;
		command["bytes_response"] = this->m_bytes_response;
		command["decoder"] = this->m_decoder;
		command["min_unit"] = this->m_min_unit;
		command["max_unit"] = this->m_max_unit;
		command["units"] = this->m_units;
		command["type_data"] = this->m_type_data;
		
		return command;
	}

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
private:
      // Datos miembro de la clase "Commands"
	std::string m_name;
	std::string m_description;
	std::string m_cmd;
	int m_bytes_response;
	std::string m_decoder;
	float m_min_unit;
	float m_max_unit;
	std::string m_units;
	std::string m_type_data;
};


#endif