#include <stdio.h>
#include <string.h>
#include "decoders.h"


int main(int argc, char const *argv[])
{
	/*char *prueba1 = "0B48";
	char *prueba2 = "0AD6";

	//printf("Valor 1 = %s \t Valor 2 = %s\n", prueba1, prueba2);

	printf("Valor RPM 1 = %.2f \t Valor RPM 2 = %.2f\n", decodeRPM(prueba1), decodeRPM(prueba2));
*/
/*
	char *response = "0B0e0C0f";

	struct RelacionesResponse datos;

	datos = decodeRelaciones(response);

	printf("Debería salir A = 11 , B = 14 , C = 12 , D = 150\n");
	printf("A = %d , B = %d ,C = %d , D = %d\n", datos.A, datos.B, datos.C, datos.D);

*/
	char *prueba = "486B10410C12F214";
	printf("Lon = %d\n", strlen(prueba));
	char *ocurrencia = strstr(prueba, "410C");
	char info[5];
	memset(info, '\0', sizeof(info));
	strncpy(info, ocurrencia + 4 , 4);
	if ( ocurrencia != NULL){
		printf("Encontrado: %s\n", info);
		printf("RPM del motor = %.2f rpm\n", decodeRPM(info));
	} else
		printf("No encontrado\n");



	return 0;
}
