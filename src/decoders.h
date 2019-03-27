#ifndef DECODERS_H
#define DECODERS_H

//Declaración estructuras

struct OxigenoResponse {
	float A;
	float B;
};
// Nota: realmente se podría utilizar la estructura OxigenoResponse
struct RelacionCombAireResponse {
	float AB;
	float CD;
};

struct RelacionesResponse {
	int A;
	int B;
	int C;
	int D;
};

//Declaración de funciones

float decodeCargaPosicionEGR(char *response);
int decodeTempGeneral(char *response);
float decodeAjusteCombustibleEGR(char *response);
int decodePresionCombustible(char *response);
int decodeHexToDec(char *response);
float decodeRPM(char *response);
float decodeAvanceTiempo(char *response);
float decodeVelocidadMAF(char *response);
struct OxigenoResponse decodeSensorOxigeno(char *response);
float decodePresionCombColector(char *response);
float decodePresionMedidorCombustible(char *response);
struct RelacionCombAireResponse decodeRelacionCombAire(char *response);
float decodePresionVapor(char *response);
struct RelacionCombAireResponse decodeRelacionCombAireActual(char *response);
float decodeTempCatalizador(char *response);
float decodeVoltajeControl(char *response);
float decodeRelacionCombAireBasica(char *response);
struct RelacionesResponse decodeRelaciones(char *response);

#endif