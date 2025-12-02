#pragma once

// Debugging
#define DEBUG 0 // Set to 1 to enable debug features (e.g., always assume a peer is present)
#define DEBUG_LOGGING 0 // Set to 1 to enable debug output via serial
#if DEBUG_LOGGING
#define GET_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define DEBUG_PRINT(...) GET_MACRO(__VA_ARGS__, DEBUG_PRINT_6, DEBUG_PRINT_5, DEBUG_PRINT_4, DEBUG_PRINT_3, DEBUG_PRINT_2, DEBUG_PRINT_1)(__VA_ARGS__)
#define DEBUG_PRINT_1(a)  Serial.println(a)
#define DEBUG_PRINT_2(a,b)  Serial.print(a);DEBUG_PRINT_1(b)
#define DEBUG_PRINT_3(a,b,c)  Serial.print(a);DEBUG_PRINT_2(b,c)
#define DEBUG_PRINT_4(a,b,c,d)  Serial.print(a);DEBUG_PRINT_3(b,c,d)
#define DEBUG_PRINT_5(a,b,c,d,e)  Serial.print(a);DEBUG_PRINT_4(b,c,d,e)
#define DEBUG_PRINT_6(a,b,c,d,e,f) Serial.print(a);DEBUG_PRINT_5(b,c,d,e,f)
#else
#define DEBUG_PRINT(...)
#endif
