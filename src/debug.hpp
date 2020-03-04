#include <stdio.h>

#ifndef DEBUG_HPP
#define DEBUG_HPP
#ifdef DEBUG
 	#define debugLog(info, args...) \
		fprintf (stderr, "[%s %s][LOG][%s][%s][Line %i] ", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__); \
		fprintf (stderr, info "\n", ##args);
	#define debugError(format, args...) \
        fprintf (stderr, "[%s %s][ERROR][%s][%s][Line %i] ", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__); \
        fprintf (stderr, info "\n", ##args);
#else
	#define debugLog(format, args...) do{} while(0);
	#define debugError(format, args...) do{} while(0);
#endif
#endif