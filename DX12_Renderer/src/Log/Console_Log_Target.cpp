#include "Console_Log_Target.h"
#include <iostream>

Console_Log_Target::Console_Log_Target():
    I_Log_Target()
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

void Console_Log_Target::log_info(Log_Info& log_info)
{
    std::cout << log_info.message <<std::endl;
}
