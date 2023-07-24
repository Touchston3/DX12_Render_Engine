#pragma once
#include "Log_Entry.h"


enum class Log_Target
{
    Console = 0,
    Debugger,
    File,
};

//TODO: Add color configuration for log targets

class I_Log_Target
{
public:
    I_Log_Target(std::wstring target_name);
    virtual ~I_Log_Target() = default;

    virtual void log_info(const ILog_Entry& log_info) = 0;
    //virtual void set_severity_level();

    std::wstring get_target_name() const { return m_target_name; }
private:
    //virtual void configure_colors();
    //Translate severity levels
    //Translate color codes?
private:
    const std::wstring m_target_name;
};
