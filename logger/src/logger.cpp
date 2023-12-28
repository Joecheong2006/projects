#include "logger.h"

#if defined(WIN32) || defined(_WIN32)
namespace Log {
    void SetColor(int colorPattern) {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, colorPattern);
    }

    void SetColor(ColorPattern colorPattern) { 
        SetColor((unsigned char)colorPattern);
    }
}
#else
#include <stdlib.h>
namespace Log {
    void SetColor(unsigned char colorPattern) { 
        std::string pattern = "\033[1;" + std::to_string(colorPattern) + "m";
        Pattern<std::string>::Log(pattern); 
    }

    void SetColor(ColorPattern colorPattern) { 
        SetColor((unsigned char)colorPattern);
    }
}
#endif

namespace Log {
    const std::string find_string_between(const std::string& str, char a, char b) {
        std::string result;
        auto fristIndex = str.find(a);
        auto endIndex = str.find(b);
        for (size_t i = fristIndex + 1; i < endIndex; ++i)
            result += str[i];
        return result;
    }

    void basic_log(const std::string& pattern) {
        Pattern<std::string>::Log(pattern);
    }
    
    void Log(ColorPattern colorPattern, const std::string& pattern) {
        SetColor(colorPattern);
        basic_log(pattern);
        SetColor();
    }
}

