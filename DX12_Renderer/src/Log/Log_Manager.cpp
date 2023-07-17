#include "Log_Manager.h"
#include "Log_Info.h"
#include "Log_Target.h"
#include "Console_Log_Target.h"
#include "Debugger_Log_Target.h"

namespace void_renderer
{
    Log_Manager::~Log_Manager()
    {
        for(auto iterator = m_log_targets.begin(); iterator != m_log_targets.end(); ++iterator)
        {
            delete(*iterator);
        }
    }

    void Log_Manager::log(Log_Info& log_info)
    {
        //Determine if we should log this info.
        if( log_info.m_log_level <= m_application_log_level )
        {
            //If we should then log to each active log target
            for(auto iterator = m_log_targets.begin(); iterator != m_log_targets.end(); ++iterator)
            {
                (*iterator)->log_info(log_info);
            }
        }
    }

    void Log_Manager::add_log_target(Log_Target log_target, std::wstring target_name)
    {
        switch(log_target)
        {
        case Log_Target::Console:
            {
                m_log_targets.push_back(new Console_Log_Target(target_name));
                break;
            }
        case Log_Target::Debugger:
            {
                m_log_targets.push_back(new Debugger_Log_Target(target_name));
                break;
            }
        case Log_Target::File:
            break;
        }
    }

    Log_Manager::Log_Manager() :
        m_application_log_level(Log_Level::None)
    {
    
    }
}
