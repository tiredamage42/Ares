#pragma once

#ifdef ARES_PLATFORM_WINDOWS
    #ifdef ARES_BUILD_DLL
        #define ARES_API __declspec(dllexport)
    #else 
        #define ARES_API __declspec(dllimport)
    #endif

#else 
    #error Ares only supports Windows for now...
#endif

#define BIT(x) (1 << x)