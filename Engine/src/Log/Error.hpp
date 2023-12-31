#pragma once
#include "Log.hpp"
#include <functional>

//TODO: Make these functions able to handle multiple error/result types. 

//Actually, I think maybe the callbacks should be called regardless of error status? IDK. 
namespace VE::Error{
	inline HRESULT handle(int line_number, const std::wstring& function_name, const std::wstring& file_name, HRESULT result)
	{
		if(FAILED(result))
		{
			Log::Log_Builder(line_number, function_name, file_name, Log::Log_Level::Error).add_log(result).print();
		}
		return result;
	}
	inline HRESULT handle(int line_number, const std::wstring& function_name, const std::wstring& file_name, HRESULT result, std::function<HRESULT(HRESULT)> callback)
	{
		if(FAILED(result))
		{
			Log::Log_Builder(line_number, function_name, file_name, Log::Log_Level::Error).add_log(result).print();
			return callback(result);
		}
		return result;
	}
	inline HRESULT handle(int line_number, const std::wstring& function_name, const std::wstring& file_name, HRESULT result, std::function<HRESULT(HRESULT, Log::Log_Builder&)> callback)
	{
		if (FAILED(result))
			return callback(result, Log::Log_Builder(line_number, function_name, file_name, Log::Log_Level::Error).add_log(result));
		return result;
	}

	inline void custom_assert(int line_number, const std::wstring& function_name, const std::wstring& file_name, HRESULT result)
	{
		if(FAILED(result))
		{
			Log::Log_Builder(line_number, function_name, file_name, Log::Log_Level::Fatal).add_log(result).print();
			std::terminate();
		}
	}
	inline void custom_assert(int line_number, const std::wstring& function_name, const std::wstring& file_name, HRESULT result, std::function<void(HRESULT)> callback)
	{
		if(FAILED(result))
		{
			Log::Log_Builder(line_number, function_name, file_name, Log::Log_Level::Fatal).add_log(result).print();
			callback(result);
			std::terminate();
		}
	}
	inline void custom_assert(int line_number, const std::wstring& function_name, const std::wstring& file_name, HRESULT result, std::function<void(HRESULT, Log::Log_Builder&)> callback)
	{
		if(FAILED(result))
		{
			callback(result, Log::Log_Builder(line_number, function_name, file_name, Log::Log_Level::Fatal).add_log(result));
			std::terminate();
		}
	}
}
#define VOID_HANDLE(result, ...) VE::Error::handle(__LINE__, __FUNCTIONW__, __FILEW__, result, ##__VA_ARGS__)

#ifdef _DEBUG
#define DEBUG_ASSERT(result, ...) VE::Error::custom_assert(__LINE__, __FUNCTIONW__, __FILEW__, result, ##__VA_ARGS__)
#else
#define DEBUG_ASSERT(result, ...) result
#endif

