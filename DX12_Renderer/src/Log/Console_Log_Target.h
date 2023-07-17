#pragma once
#include "Log_Target.h"
#include <windows.h>
class Console_Log_Target : I_Log_Target
{
public:
    Console_Log_Target();
    ~Console_Log_Target() override;

    void log_info(Log_Info& log_info) override;

private:
    bool m_embedded = false;
    HANDLE m_console_handle;
};
