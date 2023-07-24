#pragma once
#include "Log_Target.h"

class Debugger_Log_Target : public I_Log_Target
{
public:
    Debugger_Log_Target(std::wstring target_name);
    ~Debugger_Log_Target() override;

    void log_info(const ILog_Entry& log_info) override; 
};
