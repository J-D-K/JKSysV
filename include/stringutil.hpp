#pragma once
#include <string>

namespace stringutil
{
    /// @brief Returns a formatted C++ string.
    std::string get_formatted_string(const char *format, ...);
}