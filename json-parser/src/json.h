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
        error(const std::string& msg, ErrorType type, unsigned rows = -1, unsigned cols = -1)
            : msg(msg), type(type), rows(rows), cols(cols) {}
        error(const error& err, unsigned rows = -1, unsigned cols = -1)
            : msg(err.msg), type(err.type), rows(rows), cols(cols) {}
        std::string msg = "";
        ErrorType type;
        unsigned rows, cols;
        friend std::ostream& operator<<(std::ostream& os, const error& err);
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
