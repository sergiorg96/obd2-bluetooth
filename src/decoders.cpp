#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decoders.hpp"

/*
Definición de la función
*/
	//Modo 01-> Descripcion - PID - Valor Mínimo - Valor Máximo - Unidad - Fórmula
//00
//01
//02
//03
//04 - Carga calculada del motor , 0 , 100 , % , A/2.55 

float decodeCargaPosicionEGR(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/2.55;
}

//05 - Temperatura del líquido de enfriamiento del motor , -40 , 215 , ºC , A-40

int decodeTempGeneral(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec-40;
}

//06,07,08,09 - Ajuste de combustible a corto/largo plazo—Banco [1,2] , -100 (Reduccción de combustible: muy rico) , 99.2 (Aumento de combustible: muy magro) , % , A/1.28-100  

float decodeAjusteCombustibleEGR(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return (dec/1.28)-100;
}

//0a - Presión del combustible , 0 , 765 , kPa , 3A

int decodePresionCombustible(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return 3*dec;
}

//0b - Presión absoluta del colector de admisión , 0 , 255 , kPa , A

int decodeHexToDec(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec;
}

//0c - RPM del motor , 0 , 16,383.75 , rpm , (256A+B)/4 

float decodeRPM(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/4;
}

//0d - Velocidad del vehículo , 0 , 255 , km/h , A

//decodeHexToDec

//0e - Avance del tiempo , -64 , 63.5 , ° antes TDC , A/2-64 

float decodeAvanceTiempo(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return (dec/2)-64;
}

//0f - Temperatura del aire del colector de admisión , -40 , 215 , ºC, A-40

//decodeTempGeneral

//10 - Velocidad del flujo del aire MAF , 0 , 655.35  , gr/sec , (256A+B)/100 

float decodeVelocidadMAF(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/100;
}

//11 - Posición del acelerador , 0 , 100 , % , A/2.55 

//decodeCargaPosicionEGR


//12 - Estado del aire secundario controlado 
//13 - Presencia de sensores de oxígeno (en 2 bancos) 
//14,15,16,17,18,19,1a,1b - Sensor de oxígeno 1-8 A: Voltaje B: Ajuste de combustible a corto plazo , 0 100 , 1.275 99.2 , voltios % , A: A/200 B: B/1.28-100 (Si B==FF, entonces el sensor no se usa en el cálculo del ajuste)  

struct OxigenoResponse decodeSensorOxigeno(char *response){
	char AResponse[3], BResponse[3];
	float A, B;
	struct OxigenoResponse datos; 
	//Añade caracter \0 al final de la cadena AResponse
	memset(AResponse, '\0', sizeof(AResponse));
	//Divide los bytes de respuesta
	strncpy( AResponse, response, 2);
	strcpy( BResponse, response + 2 );

	A = (int)strtol(AResponse, NULL, 16);
	datos.A = A/200;
	if (strcmp(BResponse, "FF") == 0){
		//Si B==FF, entonces el sensor no se usa en el cálculo del ajuste
		datos.B=0;
	} else {
		//Si B!=FF, se aplica fórmula
		B = (int)strtol(BResponse, NULL, 16);
		datos.B = (B/1.28)-100;
	} 
	
	return datos;
}

//1c - Estándar OBD implementado en este vehículo , 
//1d - Sensores de oxígenos presentes en el banco 4
//1e - Estado de las entradas auxiliares 
//1f - Tiempo desde que se puso en marcha el motor , 0 , 65,535 , sec , 256A+B 

//decodeHexToDec

//20 - PID implementados [21 - 40] 
//21 - Distancia recorrida con la luz indicadora de falla (Malfunction Indicator Lamp, MIL) encendida , 0 , 65,535 , km

//decodeHexToDec

//22 - Presión del tren de combustible, relativa al colector de vacío , 0 , 5177.265 , kPa, 0.079(256A+B)

float decodePresionCombColector(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return 0.079*dec;
}

//23 - Presión del medidor del tren de combustible (Diesel o inyección directa de gasolina) , 0 , 655,350 , kPa , 10(256A+B) 

float decodePresionMedidorCombustible(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return 10*dec;
}

//24,25,26,27,28,29,2a,2b - Sensor de oxígeno 1 AB: Relación equivalente de combustible - aire CD: Voltaje , 0 0 , <2 <8 , prop. V , A, B: (256A+B)/32768 C, D: (256C+D)/8192   

struct RelacionCombAireResponse decodeRelacionCombAire(char *response){
	char ABResponse[5], CDResponse[5];
	float AB, CD;
	struct RelacionCombAireResponse datos; 
	//Añade caracter \0 al final de la cadena AResponse
	memset(ABResponse, '\0', sizeof(ABResponse));
	//Divide los bytes de respuesta
	strncpy( ABResponse, response, 4);
	strcpy( CDResponse, response + 4 );

	AB = (int)strtol(ABResponse, NULL, 16);
	datos.AB = AB/32768;
	CD = (int)strtol(CDResponse, NULL, 16);
	datos.CD = CD/8192; 
	
	return datos;
}

//2c - EGR comandado , 0 , 100 , % , A/2.55 

//decodeCargaPosicionEGR

//2d - falla EGR , -100 , 99.2 , % , A/1.28-100 

//decodeAjusteCombustibleEGR(

//2e - Purga evaporativa comandada , 0 , 100 , % , A/2.55 

//decodeCargaPosicionEGR

//2f - Nivel de entrada del tanque de combustible , 0 , 100 , % , A/2.55 

//decodeCargaPosicionEGR

//30 - Cantidad de calentamientos desde que se borraron los fallas , 0 , 255 , cuenta, A

//decodeHexToDec

//31 - Distancia recorrida desde que se borraron los fallas , 0 , 65,535 , km , 256A+B

//decodeHexToDec

//32 - Presión de vapor del sistema evaporativo , -8,192 , 8191.75 , Pa , (256A + B) / 4 - 8192 

float decodePresionVapor(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return (dec/4)-8192;
}

//33 - Presión barométrica absoluta , 0 , 255 , kPa , A

//decodeHexToDec

//34,35,36,37,38,39,3a,3b - Sensor de oxígeno 8 AB: Relación equivalente de combustible - aire CD: Actual , 0 -128 , <2 <128, prop. mA, A, B: (256A+B)/32768 C, D: C+D/256-128 

struct RelacionCombAireResponse decodeRelacionCombAireActual(char *response){
	char ABResponse[5], CResponse[3], DResponse[3];
	float AB, C, D;
	struct RelacionCombAireResponse datos; 
	//Añade caracter \0 al final de la cadena AResponse
	memset(ABResponse, '\0', sizeof(ABResponse));
	memset(CResponse, '\0', sizeof(CResponse));
	//Divide los bytes de respuesta
	strncpy( ABResponse, response, 4);
	strncpy( CResponse, response + 4, 2);
	strcpy( DResponse, response + 6);

	AB = (int)strtol(ABResponse, NULL, 16);
	datos.AB = AB/32768;
	C = (int)strtol(CResponse, NULL, 16);
	D = (int)strtol(DResponse, NULL, 16);
	datos.CD = C+(D/256)-128; 
	
	return datos;
}

//3c,3d,3e,3f - Temperatura del catalizador: Banco 1-4, Sensor , -40 , 6,513.5 , ºC , (256A+B)/10-40 

float decodeTempCatalizador(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return (dec/10)-40;
}

//40 - PID implementados [41 - 60]
//41 - Estado de los monitores en este ciclo de manejo 
//42 - Voltaje del módulo de control , 0 , 65.535 , V , (256A+B)/1000 

float decodeVoltajeControl(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/1000;
}

//43 - Valor absoluta de carga , 0 , 25,700 , % , (256A+B)/2.55 

//decodeCargaPosicionEGR

//44 - Relación equivaliente comandada de combustible - aire , 0 , <2 , prop. , (256A+B)/32768 

float decodeRelacionCombAireBasica(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/32768;
}

//45 - Posición relativa del acelerador , 0 , 100 , % , A/2.55 

//decodeCargaPosicionEGR

//46 - Temperatura del aire ambiental , -40 , 215 , ºC , A-40 

//decodeTempGeneral

//47,48,49,4a,4b 4c - Posición absoluta del acelerador B,C,D,E,F  Actuador comandando del acelerador , 0 , 100 , % , A/2.55  

//decodeCargaPosicionEGR

//4d, 4e - Tiempo transcurrido con MIL encendido Tiempo transcurrido desde que se borraron los códigops de fallas , 0 , 65,535 , min , 256A+B 

/*4f Valor máximo de la relación de equivalencia de combustible - aire,
 voltaje del sensor de oxígenos, corriente del sensor de oxígenos
  y presión absoluta del colector de entrada , 0 0 0 0 , 255 255 255 2550, prop. V mA kPa, A B C D*10   
*/

struct RelacionesResponse decodeRelaciones(char *response){
	char AResponse[3], BResponse[3], CResponse[3], DResponse[3];
	struct RelacionesResponse datos; 
	//Añade caracter \0 al final de la cadena AResponse
	memset(AResponse, '\0', sizeof(AResponse));
	memset(BResponse, '\0', sizeof(BResponse));
	memset(CResponse, '\0', sizeof(CResponse));
	//Divide los bytes de respuesta
	strncpy( AResponse, response, 2);
	strncpy( BResponse, response + 2, 2);
	strncpy( CResponse, response + 4, 2);
	strcpy( DResponse, response + 6);

	datos.A = (int)strtol(AResponse, NULL, 16);
	datos.B = (int)strtol(BResponse, NULL, 16);
	datos.C = (int)strtol(CResponse, NULL, 16);
	datos.D = 10*(int)strtol(DResponse, NULL, 16);
	
	return datos;
}