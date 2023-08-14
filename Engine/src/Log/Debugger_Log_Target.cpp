#include "Debugger_Log_Target.h"
#include <windows.h>

using namespace VE::Log;
Debugger_Log_Target::Debugger_Log_Target(std::wstring target_name) :
    ILog_Target(target_name)
{
}

Debugger_Log_Target::~Debugger_Log_Target()
{
}

void Debugger_Log_Target::log_info(const ILog_Entry& log_info)
{
    OutputDebugStringW(log_info.format().c_str());
}
