//
// Basic instrumentation profiler by Cherno

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Profiling::BeginSession("Session Name");        // Begin session 
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Profiling::EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use things like __FUNCSIG__ for the profile name.
//
#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

namespace Ares {

    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    //struct ProfilingSession
    //{
    //    std::string Name;
    //};

    class Profiling
    {
    private:
        //ProfilingSession* m_CurrentSession;
        inline static std::ofstream m_OutputStream;
        inline static int m_ProfileCount;// = 0;
    public:
        //Profiling() : 
            //m_CurrentSession(nullptr), 
            //m_ProfileCount(0)
        /*{
        }*/

        static void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            m_OutputStream.open(filepath);
            WriteHeader();

            m_ProfileCount = 0;
            //m_CurrentSession = new ProfilingSession{ name };
        }

        static void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            //delete m_CurrentSession;
            //m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        static void WriteProfile(const ProfileResult& result)
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }

        static void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        static void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        /*static Profiling& Get()
        {
            static Profiling instance;
            return instance;
        }*/
    };

    class Profiler
    {
    public:
        Profiler(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~Profiler()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Profiling::WriteProfile({ m_Name, start, end, threadID });

            m_Stopped = true;
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}


#define ARES_PROFILING 1

#if ARES_PROFILING
    // Resolve which function signature macro will be used. Note that this only
    // is resolved when the (pre)compiler starts, so the syntax highlighting
    // could mark the wrong one in your editor!
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define ARES_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define HZ_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__FUNCSIG__)
        #define ARES_FUNC_SIG __FUNCSIG__
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define ARES_FUNC_SIG __FUNCTION__
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define ARES_FUNC_SIG __FUNC__
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define ARES_FUNC_SIG __func__
    #elif defined(__cplusplus) && (__cplusplus >= 201103)
        #define ARES_FUNC_SIG __func__
    #else
        #define ARES_FUNC_SIG "HZ_FUNC_SIG unknown!"
    #endif

    #define ARES_PROFILE_BEGIN_SESSION(name, filepath) ::Ares::Profiling::BeginSession(name, filepath)
    #define ARES_PROFILE_END_SESSION() ::Ares::Profiling::EndSession()
    #define ARES_PROFILE_SCOPE(name) ::Ares::Profiler timer##__LINE__(name);
    //#define ARES_PROFILE_FUNCTION() ARES_PROFILE_SCOPE(__FUNCSIG__)
    #define ARES_PROFILE_FUNCTION() ARES_PROFILE_SCOPE(ARES_FUNC_SIG)
#else
    #define ARES_PROFILE_BEGIN_SESSION(name, filepath)
    #define ARES_PROFILE_END_SESSION()
    #define ARES_PROFILE_SCOPE(name)
    #define ARES_PROFILE_FUNCTION()
#endif