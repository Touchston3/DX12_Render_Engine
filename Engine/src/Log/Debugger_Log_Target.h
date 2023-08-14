#pragma once
#include "ILog_Target.h"

namespace VE::Log
{
    class Debugger_Log_Target : public ILog_Target
    {
    public:
        Debugger_Log_Target(std::wstring target_name);
        ~Debugger_Log_Target() override;

        void log_info(const ILog_Entry& log_info) override;
    };
}
