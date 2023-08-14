#include "Log_Builder.h"
#include "Log_Manager.h"

using namespace VE::Log;
Log_Builder& Log_Builder::add_log(HRESULT hresult)
{
    Graphics_Error_Log_Entry* graphics_error_log = new Graphics_Error_Log_Entry(m_line_number, m_function_name, m_file_name, m_log_level, hresult);
    m_log_entries.push_back(graphics_error_log); 
    return *this;
}
Log_Builder& Log_Builder::add_log(const std::wstring& message)
{
	Log_Entry* log_entry = new Log_Entry(m_line_number, m_function_name, m_file_name, m_log_level, message);
	m_log_entries.push_back(log_entry);
	return *this;
}


void Log_Builder::print()
{
    Log_Manager::get()->log(m_log_entries);
}

Log_Builder::Log_Builder(int line_number, const std::wstring& function_name, const std::wstring& file_name, Log_Level log_level) :
    m_line_number(line_number),
    m_function_name(function_name),
    m_file_name(file_name),
    m_log_level(log_level),
	m_log_entries({})
{
}
Log_Builder::~Log_Builder()
{
	for(auto iterator = m_log_entries.begin(); iterator != m_log_entries.end(); ++iterator)
	{
        delete(*iterator);
	}
}

