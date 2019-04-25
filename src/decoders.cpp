#include "decoders.hpp"

/*
Definición de la función
*/

//Modo 09

std::string decodeVIN(char * response){
	std::string bytes_res(response);
	std::string vin;

	//División de orden y datos
	std::string order = bytes_res.substr(0,2);
	std::string vin_bytes = bytes_res.substr(2,8);
	//Eliminación de primeros bytes de rellenos con 00
	if(!order.compare("01")){
		vin_bytes = vin_bytes.substr(6,2);
	}
	for (uint32_t i = 0; i < vin_bytes.size(); i+=2){
		std::string vin_char = vin_bytes.substr(i,2);
		//Conversión de bytes en char
		vin.push_back((char) stoi(vin_char,nullptr,16));
	}
	return vin; 
}

std::string convertDTCs(std::string dtc){
	if(dtc[0] == '0'){
		dtc.replace(0,1,"P0");
	} else if (dtc[0] == '1'){
		dtc.replace(0,1,"P1");
	} else if (dtc[0] == '2'){
		dtc.replace(0,1,"P2");
	} else if (dtc[0] == '3'){
		dtc.replace(0,1,"P3");
	} else if (dtc[0] == '4'){
		dtc.replace(0,1,"C0");
	} else if (dtc[0] == '5'){
		dtc.replace(0,1,"C1");
	} else if (dtc[0] == '6'){
		dtc.replace(0,1,"C2");
	} else if (dtc[0] == '7'){
		dtc.replace(0,1,"C3");
	} else if (dtc[0] == '8'){
		dtc.replace(0,1,"B0");
	} else if (dtc[0] == '9'){
		dtc.replace(0,1,"B1");
	} else if (dtc[0] == 'A'){
		dtc.replace(0,1,"B2");
	} else if (dtc[0] == 'B'){
		dtc.replace(0,1,"B3");
	} else if (dtc[0] == 'C'){
		dtc.replace(0,1,"U0");
	} else if (dtc[0] == 'D'){
		dtc.replace(0,1,"U1");
	} else if (dtc[0] == 'E'){
		dtc.replace(0,1,"U2");
	} else if (dtc[0] == 'F'){
		dtc.replace(0,1,"U3");
	}

	return dtc;
}

//Modo 03
std::vector<std::string> decodeDTCs(char *response){
	std::vector<std::string> vec_dtcs;
	std::string bytes_res(response);

	std::string dtc_1 = bytes_res.substr(0,4);
	if (dtc_1.compare("0000")){
		dtc_1 = convertDTCs(dtc_1);
		vec_dtcs.push_back(dtc_1);
	}
	std::string dtc_2 = bytes_res.substr(4,4);
	if (dtc_2.compare("0000")){
		dtc_2 = convertDTCs(dtc_2);
		vec_dtcs.push_back(dtc_2);
	}
	std::string dtc_3 = bytes_res.substr(8,4);
	if (dtc_3.compare("0000")){
		dtc_3 = convertDTCs(dtc_3);
		vec_dtcs.push_back(dtc_3);
	}

	return vec_dtcs;
}

	//Modo 01-> Descripcion - PID - Valor Mínimo - Valor Máximo - Unidad - Fórmula
//00 - PIDs implementados [01 - 20] -Cada bit indica si los siguientes 32 PID están implementados (1) o no (0): [A7..D0] == [PID 01..20] 

std::vector<int> decodePIDS(char *response){
	//Conversión a long para poder convertirlo a bitset
	long value_rcv = std::stol(response, nullptr, 16);
	//Conversión a bitset
	std::bitset<PID_BITS> setBit (value_rcv);
	std::vector<int> vec_pids;
	//Comprobación de PIDs disponibles(bitset lectura al reves)
	for (int i = PID_BITS-1; i >= 0; i--){
		if(setBit[i]){
			vec_pids.push_back(PID_BITS-i);
		}
	}
	return vec_pids;
}

//01 - Estado de los monitores de diagnóstico desde que se borraron los códigos de fallas DTC; incluye el estado de la luz indicadora de fallas, MIL, y la cantidad de códigos de fallas DTC 
std::map<std::string, std::string> decodeStatus(char *response){
	std::map<std::string, std::string> status;

	std::string bytes_res(response);
	std::string responseA = bytes_res.substr(0,2);
	std::string responseB = bytes_res.substr(2,2);
	std::string responseC = bytes_res.substr(4,2);
	std::string responseD = bytes_res.substr(6,2);
	
	int intA = std::stoi(responseA, nullptr, 16);
	int intB = std::stoi(responseB, nullptr, 16);
	int intC = std::stoi(responseC, nullptr, 16);
	int intD = std::stoi(responseD, nullptr, 16);
	
	std::bitset<STATUS_BITS> byteA (intA);
	std::bitset<STATUS_BITS> byteB (intB);
	std::bitset<STATUS_BITS> byteC (intC);
	std::bitset<STATUS_BITS> byteD (intD);

	if (byteA[7])
		status["MIL"] = "Encendida";
	else
		status["MIL"] = "Apagada";

	status["DTC_CNT"] = std::to_string(intA-128);

	if (byteB[0]){
		if (byteB[4])
			status["Sistema de detección de condiciones inadecuadas de ignición en cilindros"] = "Prueba Incorrecta";
		else
			status["Sistema de detección de condiciones inadecuadas de ignición en cilindros"] = "Prueba Correcta";
	}
	if (byteB[1]){
		if (byteB[5])
			status["Sistema de combustible"] = "Prueba Incorrecta";
		else
			status["Sistema de combustible"] = "Prueba Correcta";
	}
	if (byteB[2]){
		if (byteB[6])
			status["Sistema de componentes integrales"] = "Prueba Incorrecta";
		else
			status["Sistema de componentes integrales"] = "Prueba Correcta";
	}

	if (byteB[3]){
		status["IGNICION"] = "Chispa";
		if (byteC[0]){
			if (byteD[0])
				status["Sistema de eficiencia del convertidor catalítico"] = "Prueba Incorrecta";
			else
				status["Sistema de eficiencia del convertidor catalítico"] = "Prueba Correcta";
		}
		if (byteC[1]){
			if (byteD[1])
				status["Sistema de calentamiento de convertidor catalítico"] = "Prueba Incorrecta";
			else
				status["Sistema de calentamiento de convertidor catalítico"] = "Prueba Correcta";
		}
		if (byteC[2]){
			if (byteD[2])
				status["Sistema evaporativo"] = "Prueba Incorrecta";
			else
				status["Sistema evaporativo"] = "Prueba Correcta";
		}
		if (byteC[3]){
			if (byteD[3])
				status["Sistema secundario de aire"] = "Prueba Incorrecta";
			else
				status["Sistema secundario de aire"] = "Prueba Correcta";
		}
		if (byteC[4]){
			if (byteD[4])
				status["Sistema de fugas de aire acondicionado"] = "Prueba Incorrecta";
			else
				status["Sistema de fugas de aire acondicionado"] = "Prueba Correcta";
		}
		if (byteC[5]){
			if (byteD[5])
				status["Sistema de sensores de oxígeno"] = "Prueba Incorrecta";
			else
				status["Sistema de sensores de oxígeno"] = "Prueba Correcta";
		}
		if (byteC[6]){
			if (byteD[6])
				status["Sistema de calentamiento del sensor de oxígeno"] = "Prueba Incorrecta";
			else
				status["Sistema de calentamiento del sensor de oxígeno"] = "Prueba Correcta";
		}
		if (byteC[7]){
			if (byteD[7])
				status["Sistema de recirculación de los gases de escape (Exhaust Gas Recicrulation, EGR)"] = "Prueba Incorrecta";
			else
				status["Sistema de recirculación de los gases de escape (Exhaust Gas Recicrulation, EGR)"] = "Prueba Correcta";
		}
	} else {
		status["IGNICION"] = "Compresión";
		if (byteC[0]){
			if (byteD[0])
				status["Sistema de catalizador NMHC"] = "Prueba Incorrecta";
			else
				status["Sistema de catalizador NMHC"] = "Prueba Correcta";
		}
		if (byteC[1]){
			if (byteD[1])
				status["Sistema monitor de NOx/SCR"] = "Prueba Incorrecta";
			else
				status["Sistema monitor de NOx/SCR"] = "Prueba Correcta";
		}
		if (byteC[3]){
			if (byteD[3])
				status["Sistema de presión de impulso"] = "Prueba Incorrecta";
			else
				status["Sistema de presión de impulso"] = "Prueba Correcta";
		}
		if (byteC[5]){
			if (byteD[5])
				status["Sistema del sensor de gases de escape"] = "Prueba Incorrecta";
			else
				status["Sistema del sensor de gases de escape"] = "Prueba Correcta";
		}
		if (byteC[6]){
			if (byteD[6])
				status["Sistema de monitor del filtro de partículas (Particular Matter, PM)"] = "Prueba Incorrecta";
			else
				status["Sistema de monitor del filtro de partículas (Particular Matter, PM)"] = "Prueba Correcta";
		}
		if (byteC[7]){
			if (byteD[7])
				status["Sistema de recirculación de gases de escape (Exhaust Gas Recirculation, EGR) y/o VVT"] = "Prueba Incorrecta";
			else
				status["Sistema de recirculación de gases de escape (Exhaust Gas Recirculation, EGR) y/o VVT"] = "Prueba Correcta";
		}
	}

	return status;
}
//02
//03
//04 - Carga calculada del motor , 0 , 100 , % , A/2.55 

float decodeCargaPosicionEGR(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/2.55;
}

//05 - Temperatura del líquido de enfriamiento del motor , -40 , 215 , ºC , A-40

float decodeTempGeneral(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec-40;
}

//06,07,08,09 - Ajuste de combustible a corto/largo plazo—Banco [1,2] , -100 (Reduccción de combustible: muy rico) , 99.2 (Aumento de combustible: muy magro) , % , A/1.28-100  

float decodeAjusteCombustibleEGR(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return (dec/1.28)-100;
}

//0a - Presión del combustible , 0 , 765 , kPa , 3A

float decodePresionCombustible(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return 3*dec;
}

//0b - Presión absoluta del colector de admisión , 0 , 255 , kPa , A

float decodeHexToDec(char *response){
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

struct OxigenoResponse decodeRelacionCombAire(char *response){
	char ABResponse[5], CDResponse[5];
	float AB, CD;
	struct OxigenoResponse datos; 
	//Añade caracter \0 al final de la cadena AResponse
	memset(ABResponse, '\0', sizeof(ABResponse));
	//Divide los bytes de respuesta
	strncpy( ABResponse, response, 4);
	strcpy( CDResponse, response + 4 );

	AB = (int)strtol(ABResponse, NULL, 16);
	datos.A = AB/32768;
	CD = (int)strtol(CDResponse, NULL, 16);
	datos.B = CD/8192; 
	
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

struct OxigenoResponse decodeRelacionCombAireActual(char *response){
	char ABResponse[5], CResponse[3], DResponse[3];
	float AB, C, D;
	struct OxigenoResponse datos; 
	//Añade caracter \0 al final de la cadena AResponse
	memset(ABResponse, '\0', sizeof(ABResponse));
	memset(CResponse, '\0', sizeof(CResponse));
	//Divide los bytes de respuesta
	strncpy( ABResponse, response, 4);
	strncpy( CResponse, response + 4, 2);
	strcpy( DResponse, response + 6);

	AB = (int)strtol(ABResponse, NULL, 16);
	datos.A = AB/32768;
	C = (int)strtol(CResponse, NULL, 16);
	D = (int)strtol(DResponse, NULL, 16);
	datos.B = C+(D/256)-128; 
	
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
