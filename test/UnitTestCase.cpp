/** 
* @file UnitTestCase.cpp
* @author Sergio Román González
* @date 05/09/2020
* @brief Archivo que contiene el conjunto de pruebas unitarias y de integración del sistema.
*/

// Let Catch provide main():
#define CATCH_CONFIG_MAIN /**< Macro que permite a la librería catch proporcionar un main() para la ejecución del conjunto de pruebas */

#include "../src/external/catch.hpp"
#include "../src/decoders.hpp"
#include "../src/Obd.hpp"

#define BUFSIZE 30 /**< Macro del tamaño del buffer de la cadena de caracteres para el comando Minicom a ejecutar */
#define WAIT_OBDSIM 1 /**< Macro con el tiempo de espera en segundos del simulador OBDSIM para introducir valores */


using namespace Catch::literals;


/**
* @brief Función que obtiene el comando Minicom para la primera conexión con el simulador OBDSIM.
* @param cmd Puntero a cadena de caracteres para almacenar el comando de minicom a ejecutar.
*/

void getMinicomCMD(char * cmd){

    debugLog("Principio función getMinicomCMD.");

    std::string devFile = findDevPTS();

    debugLog("Dispositivo a conectarnos %s", devFile.c_str());

    snprintf(cmd, BUFSIZE, "minicom -p %s &", devFile.c_str());
}

/**
* @brief Función de inicialización del simulador OBDSIM.
* @param tipoSimulador String para indicar el tipo de simulador a iniciar.
* @param tiempoEspera Entero con el número de segundos de espera del simulador.
*
* Inicializa el simulador con entorno gráfico permitiendo la introducción de DTC y otros valores
* para las pruebas.
*/

void initOBDSIM(std::string tipoSimulador, int tiempoEspera){

    char cmd[BUFSIZE];

    debugLog("Ejecutamos OBDSIM.");

    char cmdTipoSimulador[BUFSIZE];

    snprintf(cmdTipoSimulador, BUFSIZE, "obdsim -g %s &", tipoSimulador.c_str());

    if(system(cmdTipoSimulador) == -1){
        perror("Error ejecutando comando ");
    }
    // 5 segundos para configurar parámetros para el test
    //debugLog("Esperamos la introducción de valores.");

    sleep(tiempoEspera);

    debugLog("Llamamos a la función de Minicom.");

    getMinicomCMD(cmd);

    // Se abre terminal en el test con minicom, porque el primer mensaje obdsim no envía >
    // a nivel de código, pero si con un terminal con minicom
    //system("minicom -p /dev/pts/3 &");
    debugLog("Ejecutamos minicom.");

    if(system(cmd) == -1){
        perror("Error ejecutando comando ");
    }

    sleep(1);

    debugLog("Matamos minicom.");
    // Se mata el proceso minicom, ya que, no es necesario
    if(system("pkill minicom") == -1){
        perror("Error ejecutando comando ");
    }

    sleep(1);
}

/**
* @brief Función de finalización del simulador OBDSIM.
*/

void closeOBDSIM(){
    if(system("pkill obdsim") == -1){
        perror("Error ejecutando comando ");
    }
}

/**
* @brief Prueba de integración para el funcionamiento de la obtención de DTC y PIDS disponibles.
*/

TEST_CASE( "Test OBD class DTC", "[OBD]" ) {
    
    debugLog("Comenzando test OBD class DTC");

    debugLog("Iniciamos el simulador OBDSIM");
    // Iniciamos el simulador OBDSIM para las pruebas
    initOBDSIM("Error", WAIT_OBDSIM);

    debugLog("Iniciamos conexión OBD");

	Obd connection = Obd("OBDII");

    debugLog("Finalizado el proceso de inicio de conexión OBD");

	REQUIRE (connection.isValid() == true);

    connection.printStatus();

	connection.getDTCs();

    std::cout << connection.map_commands.find("GET_DTC")->second.getJson().dump() << std::endl;

    REQUIRE ( connection.map_commands.find("GET_DTC")->second.getJson().dump() == "{\"description\":\"Get DTCs\",\"name\":\"GET_DTC\",\"units\":\"No units\",\"value\":[\"P0104\",\"B0003\",\"C0123\"]}" );

    closeOBDSIM();
}

/**
* @brief Prueba de integración para el funcionamiento de solicitud de un dato continúo (velocidad).
*/

TEST_CASE( "Test OBD class data SPEED", "[OBD]" ) {
    
    debugLog("Comenzando test OBD class data SPEED");

    debugLog("Iniciamos el simulador OBDSIM");
    // Iniciamos el simulador OBDSIM para las pruebas
    initOBDSIM("Cycle", 1);

    debugLog("Iniciamos conexión OBD");

    Obd connection = Obd("OBDII");

    debugLog("Finalizado el proceso de inicio de conexión OBD");

    REQUIRE (connection.isValid() == true);

    connection.printPIDs();

    for (int i = 0; i < 15; ++i){
        connection.send(connection.map_commands.find("SPEED")->second);

        sleep(1);

        std::cout << connection.map_commands.find("SPEED")->second.getJson().dump(4) << std::endl;
    }

    closeOBDSIM();
}

/**
* @brief Prueba unitaria del decodificador RPM.
*/

TEST_CASE( "Test Revoluciones Por Minuto", "[decoders]" ) {
    REQUIRE( decodeRPM((char *)"0000") == 0);
    REQUIRE( decodeRPM((char *)"FFFF") == Approx(16383.75).epsilon(0.01));
    REQUIRE( decodeRPM((char *)"7FFF") == Approx(8191.75).epsilon(0.01));
    REQUIRE( decodeRPM((char *)"12F2") == Approx(1212.5).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de posición EGR.
*/

TEST_CASE( "Test Posición EGR", "[decoders]" ) {
    REQUIRE( decodeCargaPosicionEGR((char *)"00") == Approx(0).epsilon(0.01));
    REQUIRE( decodeCargaPosicionEGR((char *)"FF") == Approx(100).epsilon(0.01));
    REQUIRE( decodeCargaPosicionEGR((char *)"7F") == Approx(50).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de la temperatura general.
*/

TEST_CASE( "Test Temperatura General", "[decoders]" ) {
    REQUIRE( decodeTempGeneral((char *)"00") == Approx(-40).epsilon(0.01));
    REQUIRE( decodeTempGeneral((char *)"FF") == Approx(215).epsilon(0.01));
    REQUIRE( decodeTempGeneral((char *)"7F") == Approx(87).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de la temperatura general.
*/

TEST_CASE( "Test Ajuste Combustible EGR", "[decoders]" ) {
    REQUIRE( decodeAjusteCombustibleEGR((char *)"00") == Approx(-100).epsilon(0.01));
    REQUIRE( decodeAjusteCombustibleEGR((char *)"FF") == Approx(99.2).epsilon(0.01));
    REQUIRE( decodeAjusteCombustibleEGR((char *)"7F") == Approx(-0.78125).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de la presión del combustible.
*/

TEST_CASE( "Test Presión Combustible", "[decoders]" ) {
    REQUIRE( decodePresionCombustible((char *)"00") == Approx(0).epsilon(0.01));
    REQUIRE( decodePresionCombustible((char *)"FF") == Approx(765).epsilon(0.01));
    REQUIRE( decodePresionCombustible((char *)"7F") == Approx(381).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador hexadecimal a decimal.
*/

TEST_CASE( "Test Hexadecimal a Decimal", "[decoders]" ) {
    REQUIRE( decodeHexToDec((char *)"00") == Approx(0).epsilon(0.01));
    REQUIRE( decodeHexToDec((char *)"FF") == Approx(255).epsilon(0.01));
    REQUIRE( decodeHexToDec((char *)"7F") == Approx(127).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador del avance del tiempo.
*/

TEST_CASE( "Test Avance Tiempo", "[decoders]" ) {
    REQUIRE( decodeAvanceTiempo((char *)"00") == Approx(-64).epsilon(0.01));
    REQUIRE( decodeAvanceTiempo((char *)"FF") == Approx(63.5).epsilon(0.01));
    REQUIRE( decodeAvanceTiempo((char *)"7F") == Approx(-0.5).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de la tasa de flujo del aire (MAF).
*/

TEST_CASE( "Test Velocidad Flujo Aire MAF", "[decoders]" ) {
    REQUIRE( decodeVelocidadMAF((char *)"0000") == Approx(0).epsilon(0.01));
    REQUIRE( decodeVelocidadMAF((char *)"FFFF") == Approx(655.35).epsilon(0.01));
    REQUIRE( decodeVelocidadMAF((char *)"7FFF") == Approx(327.67).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de presión de combustible relativa al colector de vacío.
*/

TEST_CASE( "Test Presión del Combustible, relativa al colector de vacío", "[decoders]" ) {
    REQUIRE( decodePresionCombColector((char *)"0000") == Approx(0).epsilon(0.01));
    REQUIRE( decodePresionCombColector((char *)"FFFF") == Approx(5177.265).epsilon(0.01));
    REQUIRE( decodePresionCombColector((char *)"7FFF") == Approx(2588.593).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de presión de combusible (Diese o inyección directa de gasolina).
*/

TEST_CASE( "Test Presión del Combustible (Diesel o inyección directa de gasolina)", "[decoders]" ) {
    REQUIRE( decodePresionMedidorCombustible((char *)"0000") == Approx(0).epsilon(0.01));
    REQUIRE( decodePresionMedidorCombustible((char *)"FFFF") == Approx(655350).epsilon(0.01));
    REQUIRE( decodePresionMedidorCombustible((char *)"7FFF") == Approx(327670).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de Vapor del Sistema Evaporativo.
*/

TEST_CASE( "Test Presión de Vapor del Sistema Evaporativo", "[decoders]" ) {
    REQUIRE( decodePresionVapor((char *)"0000") == Approx(-8192).epsilon(0.01));
    REQUIRE( decodePresionVapor((char *)"FFFF") == Approx(8191.75).epsilon(0.01));
    REQUIRE( decodePresionVapor((char *)"7FFF") == Approx(-0.25).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de Temperatura del Catalizador.
*/

TEST_CASE( "Test Temperatura del Catalizador", "[decoders]" ) {
    REQUIRE( decodeTempCatalizador((char *)"0000") == Approx(-40).epsilon(0.01));
    REQUIRE( decodeTempCatalizador((char *)"FFFF") == Approx(6513.5).epsilon(0.01));
    REQUIRE( decodeTempCatalizador((char *)"7FFF") == Approx(3236.7).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de Voltaje del Módulo de Control.
*/

TEST_CASE( "Test Voltaje del Módulo de Control", "[decoders]" ) {
    REQUIRE( decodeVoltajeControl((char *)"0000") == Approx(0).epsilon(0.01));
    REQUIRE( decodeVoltajeControl((char *)"FFFF") == Approx(65.535).epsilon(0.01));
    REQUIRE( decodeVoltajeControl((char *)"7FFF") == Approx(32.767).epsilon(0.01));
}

/**
* @brief Prueba unitaria del decodificador de Relación Equivaliente Comandada de Combustible.
*/

TEST_CASE( "Test Relación Equivaliente Comandada de Combustible", "[decoders]" ) {
    REQUIRE( decodeRelacionCombAireBasica((char *)"0000") == Approx(0).epsilon(0.01));
    REQUIRE( decodeRelacionCombAireBasica((char *)"FFFF") == Approx(2).epsilon(0.01));
    REQUIRE( decodeRelacionCombAireBasica((char *)"7FFF") == Approx(1).epsilon(0.01));
}

/**
* @brief Prueba unitaria del conversor del primer byte DTC.
*/

TEST_CASE( "Comprobación Diagnostic Trouble Codes", "[DTC]" ) {
    REQUIRE( convertDTCs("0123") == "P0123");
    REQUIRE( convertDTCs("1234") == "P1234");
    REQUIRE( convertDTCs("2345") == "P2345");
    REQUIRE( convertDTCs("3456") == "P3456");
    REQUIRE( convertDTCs("4567") == "C0567");
    REQUIRE( convertDTCs("5678") == "C1678");
    REQUIRE( convertDTCs("6789") == "C2789");
    REQUIRE( convertDTCs("789A") == "C389A");
    REQUIRE( convertDTCs("89AB") == "B09AB");
    REQUIRE( convertDTCs("9ABC") == "B1ABC");
    REQUIRE( convertDTCs("ABCD") == "B2BCD");
    REQUIRE( convertDTCs("BCDE") == "B3CDE");
    REQUIRE( convertDTCs("CDEF") == "U0DEF");
    REQUIRE( convertDTCs("DEF0") == "U1EF0");
    REQUIRE( convertDTCs("EF01") == "U2F01");
    REQUIRE( convertDTCs("F012") == "U3012");
}

/**
* @brief Prueba unitaria del Número de Identificación del vehículo.
*/

TEST_CASE( "Test VIN (Vehicle Identification Number) ISO15765-4 CAN", "[decoders]" ) {
    REQUIRE( decodeVIN((char *)"01573056\n1:3058455036384A\n2:34313430303530") == "W0V0XEP68J4140050");
    REQUIRE( decodeVIN((char *)"01314434\n1:47503030523535\n2:42313233343536") == "1D4GP00R55B123456");
}

/**
* @brief Prueba unitaria del decodificador de descriptor del protocolo actual.
*/

TEST_CASE( "Test Describir el Protocolo Actual", "[decoders]" ) {
    REQUIRE( decodeDescribeProtocol((char *)"AUTO, ISO 15765-4 (CAN 11/500)") == "AUTO, ISO 15765-4 (CAN 11/500)");
    REQUIRE( decodeDescribeProtocol((char *)"ISO 9141-2") == "ISO 9141-2");
}

/**
* @brief Escenario de pruebas con distintos test de los sensores de oxígeno.
*/

SCENARIO( "Test de Sensores de Oxígeno", "[decoders]" ) {
    GIVEN("La estructura OxigenoResponse con valores 0") {
        struct OxigenoResponse datosOX = {0,0};

        REQUIRE( datosOX.A == 0 );
        REQUIRE( datosOX.B == 0 );

        WHEN( "Valores de entrada mínimos 0x0000" ) {
            datosOX = decodeSensorOxigeno((char *)"0000");

            THEN( "Resultados tienen que ser los mínimos" ) {
                REQUIRE( datosOX.A == Approx(0).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(-100).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada máximos 0xFFFF" ) {
            datosOX = decodeSensorOxigeno((char *)"FFFF");

            THEN( "Resultados tienen que ser los máximos" ) {
                REQUIRE( datosOX.A == Approx(1.275).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(99.2).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada intermedio 0x7F7F" ) {
            datosOX = decodeSensorOxigeno((char *)"7F7F");

            THEN( "Resultados tienen que ser intermedios" ) {
                REQUIRE( datosOX.A == Approx(0.635).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(-0.78125).epsilon(0.01));
            }
        }
        WHEN( "Valores Voltaje Mínimo y Ajuste Combustible Máximo 0x00FF" ) {
            datosOX = decodeSensorOxigeno((char *)"00FF");

            THEN( "Resultados tienen que ser Voltaje Mínimo y Ajuste Combustible Máximo" ) {
                REQUIRE( datosOX.A == Approx(0).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(99.2).epsilon(0.01));
            }
        }
        WHEN( "Valores Ajuste Combustible Mínimo y Voltaje Máximo 0xFF00" ) {
            datosOX = decodeSensorOxigeno((char *)"FF00");

            THEN( "Resultados tienen que ser Ajuste Combustible Mínimo y Voltaje Máximo" ) {
                REQUIRE( datosOX.A == Approx(1.275).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(-100).epsilon(0.01));
            }
        }
    }
}

/**
* @brief Escenario de pruebas con distintos test de la relación equivalente combustible-aire.
*/

SCENARIO( "Test de Relación Equivalente Combustible-Aire", "[decoders]" ) {
    GIVEN("La estructura OxigenoResponse con valores 0") {
        struct OxigenoResponse datosOX = {0,0};

        REQUIRE( datosOX.A == 0 );
        REQUIRE( datosOX.B == 0 );

        WHEN( "Valores de entrada mínimos 0x00000000" ) {
            datosOX = decodeRelacionCombAire((char *)"00000000");

            THEN( "Resultados tienen que ser los mínimos" ) {
                REQUIRE( datosOX.A == Approx(0).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(0).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada máximos 0xFFFFFFFF" ) {
            datosOX = decodeRelacionCombAire((char *)"FFFFFFFF");

            THEN( "Resultados tienen que ser los máximos" ) {
                REQUIRE( datosOX.A == Approx(2).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(8).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada intermedio 0x7FFF7FFF" ) {
            datosOX = decodeRelacionCombAire((char *)"7FFF7FFF");

            THEN( "Resultados tienen que ser intermedios" ) {
                REQUIRE( datosOX.A == Approx(1).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(4).epsilon(0.01));
            }
        }
        WHEN( "Valores Voltaje Mínimo y Relación Equivalente Máxima 0xFFFF0000" ) {
            datosOX = decodeRelacionCombAire((char *)"FFFF0000");

            THEN( "Resultados tienen que ser Voltaje Mínimo y Relación Equivalente Máxima" ) {
                REQUIRE( datosOX.A == Approx(2).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(0).epsilon(0.01));
            }
        }
        WHEN( "Valores Relación Equivalente Mínima y Voltaje Máximo 0x0000FFFF" ) {
            datosOX = decodeRelacionCombAire((char *)"0000FFFF");

            THEN( "Resultados tienen que ser Relación Equivalente MínimA y Voltaje Máximo" ) {
                REQUIRE( datosOX.A == Approx(0).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(8).epsilon(0.01));
            }
        }
    }
}

/**
* @brief Escenario de pruebas con distintos test de la relación equivalente combustible-aire actual.
*/

SCENARIO( "Test de Relación Equivalente Combustible-Aire Actual", "[decoders]" ) {
    GIVEN("La estructura OxigenoResponse con valores 0") {
        struct OxigenoResponse datosOX = {0,0};

        REQUIRE( datosOX.A == 0 );
        REQUIRE( datosOX.B == 0 );

        WHEN( "Valores de entrada mínimos 0x00000000" ) {
            datosOX = decodeRelacionCombAireActual((char *)"00000000");

            THEN( "Resultados tienen que ser los mínimos" ) {
                REQUIRE( datosOX.A == Approx(0).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(-128).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada máximos 0xFFFFFFFF" ) {
            datosOX = decodeRelacionCombAireActual((char *)"FFFFFFFF");

            THEN( "Resultados tienen que ser los máximos" ) {
                REQUIRE( datosOX.A == Approx(2).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(128).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada intermedio 0x7FFF7FFF" ) {
            datosOX = decodeRelacionCombAireActual((char *)"7FFF7FFF");

            THEN( "Resultados tienen que ser intermedios" ) {
                REQUIRE( datosOX.A == Approx(1).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(-0.00390625).epsilon(0.01));
            }
        }
        WHEN( "Valores Corriente Mínimo y Relación Equivalente Máxima 0xFFFF0000" ) {
            datosOX = decodeRelacionCombAireActual((char *)"FFFF0000");

            THEN( "Resultados tienen que ser Corriente Mínimo y Relación Equivalente Máxima" ) {
                REQUIRE( datosOX.A == Approx(2).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(-128).epsilon(0.01));
            }
        }
        WHEN( "Valores Relación Equivalente Mínima y Corriente Máximo 0x0000FFFF" ) {
            datosOX = decodeRelacionCombAireActual((char *)"0000FFFF");

            THEN( "Resultados tienen que ser Relación Equivalente MínimA y Corriente Máximo" ) {
                REQUIRE( datosOX.A == Approx(0).epsilon(0.01));
                REQUIRE( datosOX.B == Approx(128).epsilon(0.01));
            }
        }
    }
}

/**
* @brief Escenario de pruebas con distintos test de valores máximo relación de combustible-aire, voltaje, corriente y presión absoluta.
*/

SCENARIO( "Test de Valores máximos relación de combustible-aire, voltaje, corriente y presión absoluta", "[decoders]" ) {
    GIVEN("La estructura RelacionesResponse con valores 0") {
        struct RelacionesResponse datosREL = {0,0,0,0};

        REQUIRE( datosREL.A == 0 );
        REQUIRE( datosREL.B == 0 );
        REQUIRE( datosREL.C == 0 );
        REQUIRE( datosREL.D == 0 );

        WHEN( "Valores de entrada mínimos 0x00000000" ) {
            datosREL = decodeRelaciones((char *)"00000000");

            THEN( "Resultados tienen que ser los mínimos" ) {
                REQUIRE( datosREL.A == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(0).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada máximos 0xFFFFFFFF" ) {
            datosREL = decodeRelaciones((char *)"FFFFFFFF");

            THEN( "Resultados tienen que ser los máximos" ) {
                REQUIRE( datosREL.A == Approx(255).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(255).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(255).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(2550).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada intermedio 0x7F7F7F7F" ) {
            datosREL = decodeRelaciones((char *)"7F7F7F7F");

            THEN( "Resultados tienen que ser intermedios" ) {
                REQUIRE( datosREL.A == Approx(127).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(127).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(127).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(1270).epsilon(0.01));
            }
        }
        WHEN( "Valores de entrada Relación Máxima, resto mínimo 0xFF000000" ) {
            datosREL = decodeRelaciones((char *)"FF000000");

            THEN( "Resultados tienen que ser Relación Máxima, resto mínimo" ) {
                REQUIRE( datosREL.A == Approx(255).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(0).epsilon(0.01));
            }
        }
        WHEN( "Valores Voltaje Máximo, resto mínimo 0x00FF0000" ) {
            datosREL = decodeRelaciones((char *)"00FF0000");

            THEN( "Resultados tienen que ser Voltaje Máximo, resto mínimo" ) {
                REQUIRE( datosREL.A == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(255).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(0).epsilon(0.01));
            }
        }
        WHEN( "Valores Corriente Máxima, resto mínimo 0x0000FF00" ) {
            datosREL = decodeRelaciones((char *)"0000FF00");

            THEN( "Resultados tienen que ser Corriente Máxima, resto mínimo" ) {
                REQUIRE( datosREL.A == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(255).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(0).epsilon(0.01));
            }
        }
        WHEN( "Valores Presión Máxima, resto mínimo 0x000000FF" ) {
            datosREL = decodeRelaciones((char *)"000000FF");

            THEN( "Resultados tienen que ser Presión Máxima, resto mínimo" ) {
                REQUIRE( datosREL.A == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.B == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.C == Approx(0).epsilon(0.01));
                REQUIRE( datosREL.D == Approx(2550).epsilon(0.01));
            }
        }
    }
}

/**
* @brief Escenario de pruebas con distintos test de DTC activos.
*/

SCENARIO( "Test de decodificación de Data Trouble Codes (DTC)", "[decoders]" ) {
    GIVEN("Vector vacío cuyos componentes son strings (DTC's)") {
    	std::vector<std::string> vec_dtcs;

        REQUIRE( vec_dtcs.empty() == 1 );

        WHEN( "Sólo un DTC" ) {
        	vec_dtcs = decodeDTCs((char *)"013300000000");

            THEN( "Resultado de sólo un DTC" ) {
                REQUIRE( vec_dtcs.size() == 1);
                REQUIRE( vec_dtcs[0] == "P0133");
            }
        }
        WHEN( "Dos DTC's" ) {
        	vec_dtcs = decodeDTCs((char *)"0133D0160000");

            THEN( "Resultado de dos DTC's" ) {
                REQUIRE( vec_dtcs.size() == 2);
                REQUIRE( vec_dtcs[0] == "P0133");
                REQUIRE( vec_dtcs[1] == "U1016");
            }
        }
        WHEN( "Tres DTC's" ) {
        	vec_dtcs = decodeDTCs((char *)"0133D0161131");

        	THEN( "Resultado de tres DTC's" ) {
        		REQUIRE( vec_dtcs.size() == 3);
        		REQUIRE( vec_dtcs[0] == "P0133");
        		REQUIRE( vec_dtcs[1] == "U1016");
        		REQUIRE( vec_dtcs[2] == "P1131");
        	}
        }
    }
}

/**
* @brief Escenario de pruebas con distintos test los PIDS implementados en el vehículo.
*/

SCENARIO( "Test de decodificación PIDs disponibles", "[decoders]" ) {
    GIVEN("Vector vacío cuyos componentes son int (DTC's)") {
    	std::vector<int> vec_pids;

        REQUIRE( vec_pids.empty() == 1 );

        WHEN( "PIDs impares disponibles" ) {
        	vec_pids = decodePIDS((char *)"AAAAAAAA");

            THEN( "Resultado con números impares del 1-20 (Hexadecimal, decimal 1-32)" ) {
                REQUIRE( vec_pids.size() == 16);
                int pids_impares = 1;
                for (uint32_t i = 0; i < vec_pids.size(); ++i){
                	REQUIRE( vec_pids[i] == pids_impares);
                	pids_impares+=2;

                }
            }
        }
        WHEN( "PIDs pares disponibles" ) {
        	vec_pids = decodePIDS((char *)"55555555");

            THEN( "Resultado con números pares del 1-20 (Hexadecimal, decimal 1-32)" ) {
                REQUIRE( vec_pids.size() == 16);
                int pids_pares = 2;
                for (uint32_t i = 0; i < vec_pids.size(); ++i){
                	REQUIRE( vec_pids[i] == pids_pares);
                	pids_pares+=2;

                }
            }
        }
        WHEN( "PIDs primeros disponibles" ) {
        	vec_pids = decodePIDS((char *)"FFFF0000");

            THEN( "Resultado con los primeros números del 1-20 (Hexadecimal, decimal 1-32)" ) {
                REQUIRE( vec_pids.size() == 16);
                int pids_primeros = 1;
                for (uint32_t i = 0; i < vec_pids.size(); ++i){
                	REQUIRE( vec_pids[i] == pids_primeros);
                	pids_primeros++;

                }
            }
        }
        WHEN( "PIDs ultimos disponibles" ) {
        	vec_pids = decodePIDS((char *)"0000FFFF");

            THEN( "Resultado con los ultimos números del 1-20 (Hexadecimal, decimal 1-32)" ) {
                REQUIRE( vec_pids.size() == 16);
                int pids_ultimos = 17;
                for (uint32_t i = 0; i < vec_pids.size(); ++i){
                	REQUIRE( vec_pids[i] == pids_ultimos);
                	pids_ultimos++;

                }
            }
        }
    }
}

/**
* @brief Escenario de pruebas con distintos valores de los monitores de diagnóstico tras las pruebas del vehículo.
*/

SCENARIO( "Test de decodificación del estado del coche", "[decoders]" ) {
    GIVEN("Map de <string, string> de estado vacío") {
    	std::map<std::string, std::string> status;

        REQUIRE( status.empty() == 1 );

        WHEN( "La entrada es 0x81076504" ) {
        	status = decodeStatus((char *)"81076504");

            THEN( "Resultado 1 DTC, Compresión, MIL Encendida y todas pruebas correctas" ) {
                REQUIRE( status.size() == 9);
                REQUIRE( status["MIL"] == "Encendida");
                REQUIRE( status["DTC_CNT"] == "1");
                REQUIRE( status["IGNICION"] == "Compresión");
                REQUIRE( status["Sistema de catalizador NMHC"] == "Prueba Correcta");
                REQUIRE( status["Sistema de detección de condiciones inadecuadas de ignición en cilindros"] == "Prueba Correcta");
                REQUIRE( status["Sistema de componentes integrales"] == "Prueba Correcta");
                REQUIRE( status["Sistema de combustible"] == "Prueba Correcta");
                REQUIRE( status["Sistema del sensor de gases de escape"] == "Prueba Correcta");
                REQUIRE( status["Sistema de monitor del filtro de partículas (Particular Matter, PM)"] == "Prueba Correcta");
            }
        }
    }
}