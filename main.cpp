#include "Obd.hpp"

int main()
{
	Obd connection;

	if(connection.isValid()){
		std::cout << "Se ha creado correctamente" << std::endl;
		connection.disconnectBluetooth();
	} else {
		std::cout << "No se ha conectado correctamente" << std::endl;
		std::cout << "Lista de comandos disponibles:" << std::endl << std::endl;
		std::cout << "\tComando\t\tBytes" << std::endl;
		for (int i = 0; i < (int)connection.m_commands.size(); ++i)
		{
			std::cout << "\t" << connection.m_commands[i].getName() << "\t\t" << connection.m_commands[i].getCMD() << std::endl;	
		}
	}

	return 0;
}
