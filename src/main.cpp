#include "Obd.hpp"

int main(int argc, char **argv)
{
	if (argc != 2){
		printf("Uso: ./main DEVICE-NAME\n");
		return 0;
	}

	Obd connection = Obd(argv[1]);

	if(connection.isValid()){
		std::cout << "Se ha conectado correctamente" << std::endl;

		connection.send(connection.map_commands.find("RPM")->second);


		connection.disconnectBluetooth();
	} else {
		std::cout << "No se ha conectado correctamente" << std::endl;
		/*
		std::cout << "Lista de comandos disponibles:" << std::endl << std::endl;
		std::cout << "\tComando\t\tBytes" << std::endl;
		for (int i = 0; i < (int)connection.vec_commands.size(); ++i)
		{
			std::cout << "\t" << connection.vec_commands[i].getName() << "\t\t" << connection.vec_commands[i].getCMD() << std::endl;	
		}
		*/
		//std::cout << "Comando a ejecutar: " << connection.map_commands.find("RPM")->second.getCMD() << std::endl;
	}

	return 0;
}
