#pragma once
#include "Log_Manager.h"


#define INFO_LOG_BUILDER() Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, Log_Level::Info)
#define WARNING_LOG_BUILDER() Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, Log_Level::Warning)
#define ERROR_LOG_BUILDER() Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, Log_Level::Error)
#define FATAL_LOG_BUILDER() Log_Builder(__LINE__, __FUNCTIONW__, __FILEW__, Log_Level::Fatal)

#ifdef _DEBUG


#endif


/*
	ok so how do I want to do this? We can build logs using INFO_LOG_BUILDER().add_log("log stuff").write(); That seems like exactly what I want
	How do we want to incorperate error handling now? Well, I think I want something it to be akin to handle( function call, error_callback ) 
 */
