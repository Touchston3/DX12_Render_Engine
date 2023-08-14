#pragma once
#include "Log_Manager.h"


#define INFO_LOG_BUILDER() VE::Log::Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, VE::Log::Log_Level::Info)
#define WARNING_LOG_BUILDER() VE::Log::Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, VE::Log::Log_Level::Warning)
#define ERROR_LOG_BUILDER() VE::Log::Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, VE::Log::Log_Level::Error)
#define FATAL_LOG_BUILDER() VE::Log::Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, VE::Log::Log_Level::Fatal)

//I should probably create some generic debug macros so we don't need to use the builder every time we want to log. 
#ifdef _DEBUG

#else

#endif


