#pragma once

#include <vector>
#include <string>

namespace json {
    enum ErrorType {
        InvalidInput,
        InvalidWord,
        InvalidFormat,
    };

    struct error {
        error() = default;
        error(const std::string& msg, ErrorType type): msg(msg), type(type) {}
        std::string msg = "";
        ErrorType type;
        unsigned rows, cols;
    };

    template <typename T>
    class ret_type {
    public:
        T val;
        error err;

        bool operator==(bool boolean) const {
            return err.msg.size() != boolean;
        }

        operator bool() const {
            return err.msg.size() == 0;
        }
    };

    enum TokenType {
        Number = 256,
        String,
        Null,
        Boolean,
    };

    struct token {
        int type;
        unsigned rows, cols;
        union {
            double number;
            char* literal;
            bool boolean;
        } val;
    };

    class file {
    private:
        std::string m_content;

    public:
        static ret_type<file> load(const std::string& path);
        inline const std::string& content() const { return m_content; }
    };

    struct lexer {
        static ret_type<std::vector<token>> load_file(const file& f);
    };
}
