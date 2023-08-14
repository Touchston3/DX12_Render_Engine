#pragma once
#include "ILog_Target.h"
#include <windows.h>

namespace VE::Log
{
    class Console_Log_Target : public ILog_Target
    {
    public:
        Console_Log_Target(std::wstring target_name);
        ~Console_Log_Target() override;

        void log_info(const ILog_Entry& log_info) override;

    private:
        bool m_embedded = false;
        HANDLE m_console_handle;
    };
}
