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

}
