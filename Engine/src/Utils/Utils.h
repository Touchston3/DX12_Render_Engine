#pragma once
#include <complex>
#include <string>
#include <fstream>
#include <filesystem>

namespace void_renderer::utils {

    template<typename T, typename... Args>
    void format_helper(std::wstringstream& stream, const std::wstring& base, T arg, Args... args)
    {
        size_t pos = base.find(L"{}");

        stream << base.substr(0, pos) << arg;
        format_helper(stream, base.substr(pos + 2), args...);
    }


    template<typename T>
    void format_helper(std::wstringstream& stream, const std::wstring& base, T arg)
    {
        size_t pos = base.find(L"{}");

        stream << base.substr(0, pos) << arg;
    }

    template<typename... Args>
    std::wstring format_wstring(std::wstring base, Args... args)
    {
        std::wstringstream ss;
        format_helper(ss, base, args...);
        return ss.str();
    }
    /*
	inline std::wstring narrow_to_wide(const std::string& narrow) //Credit goes to LihO and Jossef Harush Kadouri on stack overflow
    {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &narrow[0], (int)narrow.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &narrow[0], (int)narrow.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }
	*/
}
