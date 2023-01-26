#pragma once

#include "helpers/fs.h"
#include "plog/Severity.h"
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

class Logger
{
public:
    static void Init() 
    { 
        fs::Remove("tilemap-editor.txt");
        plog::init(plog::verbose, "tilemap-editor.txt"); 
    }

    template<typename ...Args>
    static void Print(plog::Severity sev, Args... args)
    {
        PLOG(sev).printf(args...);
    }
};


#define TS_LOG_NONE(...) PLOG(plog::none).printf(__VA_ARGS__)
#define TS_LOG_FATAL(...) PLOG(plog::fatal).printf(__VA_ARGS__)
#define TS_LOG_ERROR(...) PLOG(plog::none).printf(__VA_ARGS__)
#define TS_LOG_WARN(...) PLOG(plog::warning).printf(__VA_ARGS__)
#define TS_LOG_INFO(...) PLOG(plog::info).printf(__VA_ARGS__)
#define TS_LOG_DEBUG(...) PLOG(plog::debug).printf(__VA_ARGS__)
#define TS_LOG_VERBOSE(...) PLOG(plog::verbose).printf(__VA_ARGS__)


