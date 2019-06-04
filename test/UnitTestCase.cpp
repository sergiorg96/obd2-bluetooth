// 010-TestCase.cpp

// Let Catch provide main():
#define CATCH_CONFIG_MAIN

#include "../src/external/catch.hpp"
#include "../src/decoders.cpp"
/*
float decodeRPM(char *response){
	int dec = (int)strtol(response, NULL, 16);
	return dec/4;
}
*/
/*
int Factorial( int number ) {
   return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
// return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}
*/
/*
TEST_CASE( "Factorial of 0 is 1 (fail)", "[single-file]" ) {
    REQUIRE( Factorial(0) == 1 );
}

TEST_CASE( "Factorials of 1 and higher are computed (pass)", "[single-file]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
*/
//float resultado = 0x12F2/4;


TEST_CASE( "Decodificadores de valores de los sensores", "[decoders]" ) {
    REQUIRE( decodeRPM("12F2") == 0x12F2/4);
}

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



// Compile & run:
// - g++ -std=c++11 -Wall -I$(CATCH_SINGLE_INCLUDE) -o 010-TestCase 010-TestCase.cpp && 010-TestCase --success
// - cl -EHsc -I%CATCH_SINGLE_INCLUDE% 010-TestCase.cpp && 010-TestCase --success

// Expected compact output (all assertions):
//
// prompt> 010-TestCase --reporter compact --success
// 010-TestCase.cpp:14: failed: Factorial(0) == 1 for: 0 == 1
// 010-TestCase.cpp:18: passed: Factorial(1) == 1 for: 1 == 1
// 010-TestCase.cpp:19: passed: Factorial(2) == 2 for: 2 == 2
// 010-TestCase.cpp:20: passed: Factorial(3) == 6 for: 6 == 6
// 010-TestCase.cpp:21: passed: Factorial(10) == 3628800 for: 3628800 (0x375f00) == 3628800 (0x375f00)
// Failed 1 test case, failed 1 assertion.