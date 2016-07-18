#pragma once

#include "ILogger.h"
#include "IComponent.h"
#include <fstream>

namespace GlEngine
{
    class ENGINE_SHARED FileLogger : public ILogger, public IComponent
    {
    public:
        FileLogger(const char *const filename);
        FileLogger(const char *const path, const char *const filename);
        ~FileLogger();

        bool Initialize();
        void Shutdown();

        const char *name() override;

        inline rt_mutex &GetMutex()
        {
            return _mutex;
        }

    private:
        const char *const _path,
                   *const _filename;
        std::ofstream _logStream;
        rt_mutex _mutex;

        bool LogImpl(LogType type, const char *const message) override;
    };
}
