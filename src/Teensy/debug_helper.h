// debug_helper.h

#ifndef _DEBUG_HELPER_h
#define _DEBUG_HELPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define br          Serial.println()
#define comma       Serial.print(" , ")
#define sflush      Serial.flush()

template<typename T>
inline void print(T val1, T val2, T val3, String header="") {
    Serial.print(header + String(val1) + " , " + val2 + " , " + val3);
}

template<typename T>
inline void println(T val1, String header = "") {
    Serial.println(header + String(val1));
}

template<typename T>
inline void println(T val1, T val2, String header="") {
    Serial.println(header + String(val1) + " , " + val2);
}

template<typename T>
inline void println(T val1, T val2, T val3, String header = "") {
    Serial.println(header + String(val1) + " , " + val2 + " , " + val3);
}

#endif

