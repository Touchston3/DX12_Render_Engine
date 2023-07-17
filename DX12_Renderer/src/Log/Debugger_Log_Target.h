#pragma once
#include "Log_Target.h"

class Debugger_Log_Target : I_Log_Target
{
public:
    Debugger_Log_Target();
    ~Debugger_Log_Target() override;

    void log_info(Log_Info& log_info) override; 
};
