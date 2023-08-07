#include "Console_Log_Target.h"
#include <iostream>

Console_Log_Target::Console_Log_Target(std::wstring target_name):
    ILog_Target(target_name)
{
        //Check if console. Also, not sure how this works if it is supposed to be an embedded console. 
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        FILE* console_stream_handler;
        if(!freopen_s(&console_stream_handler, "CON", "w", stdout))
        {
        
        }
        m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

Console_Log_Target::~Console_Log_Target()
{
}

void Console_Log_Target::log_info(const ILog_Entry& log_info)
{
    std::wcout << log_info.format() <<std::endl;
}
