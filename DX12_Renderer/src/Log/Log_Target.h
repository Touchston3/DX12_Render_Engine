#pragma once
#include "Log_Info.h"
// Ok, so this is going to need a way to enable, a way to write, possible color config, a way to set severity
class I_Log_Target
{
public:
    I_Log_Target();
    virtual ~I_Log_Target();

    virtual void log_info(Log_Info& log_info) = 0;
    virtual void set_severity_level();
private:
    virtual void configure_colors();
    //Translate severity levels?
    //Translate color codes?
};
