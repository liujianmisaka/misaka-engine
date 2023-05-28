#pragma once

#include <optional>

namespace Hazel {
    class FileDialogs {
    public:
        // These returns empty string if cancelled
        static std::optional<std::string> OpenFile(const char* filter);
        static std::optional<std::string> SaveFile(const char* filter);
    };
}
