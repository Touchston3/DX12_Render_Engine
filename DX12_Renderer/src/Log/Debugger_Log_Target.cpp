#include "Debugger_Log_Target.h"
#include <debugapi.h>
Debugger_Log_Target::Debugger_Log_Target() :
    I_Log_Target()
{
}

Debugger_Log_Target::~Debugger_Log_Target()
{
}

void Debugger_Log_Target::log_info(Log_Info& log_info)
{
    OutputDebugStringW(log_info.message);
}
