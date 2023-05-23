#pragma once

namespace Hazel {
    class FileDialogs {
    public:
        // These returns empty string if cancelled
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}
