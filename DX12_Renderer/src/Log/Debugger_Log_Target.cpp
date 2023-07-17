#include "Debugger_Log_Target.h"
#include <windows.h>
Debugger_Log_Target::Debugger_Log_Target(std::wstring target_name) :
    I_Log_Target(target_name)
{
}

Debugger_Log_Target::~Debugger_Log_Target()
{
}

void Debugger_Log_Target::log_info(Log_Info& log_info)
{
    OutputDebugStringW(log_info.message.c_str());
}
