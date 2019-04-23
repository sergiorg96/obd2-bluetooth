#ifndef DECODERS_H
#define DECODERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <vector>

#define PID_BITS 32


//Declaración estructuras

struct OxigenoResponse {
	float A;
	float B;
};
// Nota: realmente se podría utilizar la estructura OxigenoResponse

struct RelacionesResponse {
	int A;
	int B;
	int C;
	int D;
};

//Declaración de funciones

std::vector<int> decodePIDS(char *response);
std::vector<std::string> decodeDTCs(char *response);
std::string convertDTCs(std::string dtc);
float decodeCargaPosicionEGR(char *response);
float decodeTempGeneral(char *response);
float decodeAjusteCombustibleEGR(char *response);
float decodePresionCombustible(char *response);
float decodeHexToDec(char *response);
float decodeRPM(char *response);
float decodeAvanceTiempo(char *response);
float decodeVelocidadMAF(char *response);
struct OxigenoResponse decodeSensorOxigeno(char *response);
float decodePresionCombColector(char *response);
float decodePresionMedidorCombustible(char *response);
struct OxigenoResponse decodeRelacionCombAire(char *response);
float decodePresionVapor(char *response);
struct OxigenoResponse decodeRelacionCombAireActual(char *response);
float decodeTempCatalizador(char *response);
float decodeVoltajeControl(char *response);
float decodeRelacionCombAireBasica(char *response);
struct RelacionesResponse decodeRelaciones(char *response);

#endif