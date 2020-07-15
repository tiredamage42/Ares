
#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>

namespace Ares {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult
    {
        std::string Name;
     
        FloatingPointMicroseconds Start;
        std::chrono::microseconds ElapsedTime;

        std::thread::id ThreadID;
    };

    class Profiling
    {
    private:
        inline static std::mutex m_Mutex;
        inline static std::ofstream m_OutputStream;
        inline static bool m_CurrentProfile = false;
        
    public:
        
        static void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {

            std::lock_guard lock(m_Mutex);
            if (m_CurrentProfile) {
                // If there is already a current session, then close it before beginning new one.
                // Subsequent profiling output meant for the original session will end up in the
                // newly opened session instead.  That's better than having badly formatted
                // profiling output.
                
                // Edge case: BeginSession() might be before Log::Init()
                if (Log::GetCoreLogger()) { 
                    ARES_CORE_ERROR("Profiler::BeginSession('{0}') when another session is already open.", name);
                }
                
                InternalEndSession();
            }

            m_OutputStream.open(filepath);

            if (m_OutputStream.is_open()) {
                m_CurrentProfile = true;
                WriteHeader();
            }
            else {
                // Edge case: BeginSession() might be before Log::Init()
                if (Log::GetCoreLogger()) { 
                    ARES_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
                }
            }
        }

        static void EndSession()
        {
            std::lock_guard lock(m_Mutex);
            InternalEndSession();
        }
    
        static void WriteProfile(const ProfileResult& result)
        {
            std::stringstream json;

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            json << std::setprecision(3) << std::fixed;

            json << ",{";
            json << "\"cat\":\"function\",";
            
            json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
            json << "\"name\":\"" << name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start.count();
            json << "}";

            std::lock_guard lock(m_Mutex);
            if (m_CurrentProfile) {
                m_OutputStream << json.str();
                m_OutputStream.flush();
            }
        }
    private:

        static void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();
        }

        static void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
        static void InternalEndSession() {
            if (m_CurrentProfile) {
                WriteFooter();
                m_OutputStream.close();
                m_CurrentProfile = false;
            }
        }
    };

    class Profiler
    {
    public:
        Profiler(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();
        }

        ~Profiler()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();
            auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

            Profiling::WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

            m_Stopped = true;
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
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
    #define ARES_PROFILE_FUNCTION() ARES_PROFILE_SCOPE(ARES_FUNC_SIG)
#else
    #define ARES_PROFILE_BEGIN_SESSION(name, filepath)
    #define ARES_PROFILE_END_SESSION()
    #define ARES_PROFILE_SCOPE(name)
    #define ARES_PROFILE_FUNCTION()
#endif