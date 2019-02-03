#include "json.hpp"
#include "Commands.hpp"
#include <iostream>
#include <fstream>

#include <iomanip> //std::setw

int main(){
	using json = nlohmann::json;

	std::ifstream ifs("data/PIDS.json");
	auto j = json::parse(ifs);

	std::vector<Commands> commands;

	for (int i = 0; i < (int)j.size(); ++i)
	{
		commands.push_back(Commands(j[i]));
	}
	//Commands commands[0](j[0]);

	//Commands prueba[];
/*
	for (int i = 0; i < j.size(); ++i)
	{
		prueba[i] = new Commands
	}
*/

	for (int i = 0; i < (int)commands.size(); ++i)
	{
		std::cout << "Name: " << commands[i].getName() << std::endl;
		std::cout << "Description: " << commands[i].getDescription() << std::endl;
		std::cout << "Bytes cmd: " << commands[i].getCMD() << std::endl;
		std::cout << "Bytes response: " << commands[i].getBytesResponse() << std::endl;
		std::cout << "Decoder: " << commands[i].getDecoder() << std::endl << std::endl;
		std::cout << "#######################################################" << std::endl;
	}
	/*
	auto j2 = commands[0].getJson();

	std::cout << "Name: " << (std::string)j2["name"] << std::endl;
	std::cout << "Description: " << (std::string)j2["description"] << std::endl;
	std::cout << "Bytes cmd: " << (std::string)j2["cmd"] << std::endl;
	std::cout << "Bytes response: " << (int)j2["bytes_response"] << std::endl;
	std::cout << "Decoder: " << (std::string)j2["decoder"] << std::endl;

	std::cout << j2.dump(4) << std::endl;
*/

	return 0;
}