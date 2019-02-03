#include "json.hpp"
#include <iostream>
#include <fstream>

#include <iomanip> //std::setw


int main()
{
	using json = nlohmann::json;

	std::ifstream ifs("PIDS.json");
	auto j = json::parse(ifs);

	std::cout << "Tamaño = " << j.size() << std::endl;


	for (int i = 0; i < (int)j.size(); ++i)
	{
		std::cout << "Name: " << (std::string)j[i]["name"] << std::endl;
		std::cout << "Description: " << (std::string)j[i]["description"] << std::endl;
		std::cout << "Bytes cmd: " << (std::string)j[i]["cmd"] << std::endl;
		std::cout << "Bytes response: " << (int)j[i]["bytes_response"] << std::endl;
		std::cout << "Decoder: " << (std::string)j[i]["decoder"] << std::endl;
	}

	std::cout << j.dump() << std::endl;
	std::cout << j.dump(4) << std::endl;
	//std::cout << j << std::endl; //same that j.dump()
	//std::cout << std::setw(4) << j << std::endl; //same that j.dump(4) << j

	//printf("Nombre: %s\nApellidos: %s\nEdad: %d años\nNombre mascota: %s\n",(std::string), (std::string)j["apellidos"], (int)j["edad"],(std::string)j["mascota"]);

	return 0;
}