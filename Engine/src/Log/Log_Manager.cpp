#include "Log_Manager.h"
#include "Log_Entry.h"
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

    void Log_Manager::log(std::vector<ILog_Entry*> log_entries)
    {
        //Determine if we should print this info.
        for (auto entry_iterator = log_entries.begin(); entry_iterator != log_entries.end(); ++entry_iterator)
        {
            if ((*entry_iterator)->m_log_level <= m_application_log_level)
            {
                //If we should then print to each active print target
                for (auto target_iterator = m_log_targets.begin(); target_iterator != m_log_targets.end(); ++target_iterator)
                {
                    (*target_iterator)->log_info(*(*entry_iterator));
                }
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
