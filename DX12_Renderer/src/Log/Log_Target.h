#pragma once
#include "Log_Info.h"
// Ok, so this is going to need a way to enable, a way to write, possible color config, a way to set severity
enum class Log_Target
{
    Console = 0,
    Debugger,
    File,
};

class I_Log_Target
{
public:
    I_Log_Target(std::wstring target_name);
    virtual ~I_Log_Target() = default;

    virtual void log_info(Log_Info& log_info) = 0;
    //virtual void set_severity_level();

    std::wstring get_target_name() const { return m_target_name; }
private:
    //virtual void configure_colors();
    //Translate severity levels
    //Translate color codes?
private:
    const std::wstring m_target_name;
};
