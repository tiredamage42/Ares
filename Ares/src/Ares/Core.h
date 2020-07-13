#pragma once

#include <memory>

#ifdef ARES_PLATFORM_WINDOWS
    #if ARES_DYNAMIC_LINK
        #ifdef ARES_BUILD_DLL
            #define ARES_API __declspec(dllexport)
        #else 
            #define ARES_API __declspec(dllimport)
        #endif
    #else
        #define ARES_API
    #endif
#else 
    #error Ares only supports Windows for now...
#endif

#ifdef ARES_DEBUG
    #define ARES_ENABLE_ASSERTS
#endif

#ifdef ARES_ENABLE_ASSERTS
    #define ARES_ASSERT(x, ...) { if (!(x)) { ARES_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define ARES_CORE_ASSERT(x, ...) { if (!(x)) { ARES_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define ARES_ASSERT(x, ...)
    #define ARES_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ARES_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Ares 
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    
    template<typename T>
    using Ref = std::shared_ptr<T>;
}
