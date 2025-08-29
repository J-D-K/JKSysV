#include "stringutil.hpp"

#include <array>
#include <cstdarg>

namespace
{
    constexpr int SIZE_VA_BUFFER = 0x300;
}

std::string stringutil::get_formatted_string(const char *format, ...)
{
    std::array<char, SIZE_VA_BUFFER> vaBuffer{0};

    std::va_list vaList;
    va_start(vaList, format);
    std::vsnprintf(vaBuffer.data(), SIZE_VA_BUFFER, format, vaList);
    va_end(vaList);

    return std::string{vaBuffer.data()};
}