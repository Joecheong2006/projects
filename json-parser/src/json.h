#pragma once

#include <vector>
#include <string>

namespace json {
    enum ErrorType {
        InvalidInput,
    };

    struct error {
        std::string msg = "";
        ErrorType type;
    };

    template <typename T>
    class ret_type {
    public:
        T val;
        error err;

        bool operator==(bool boolean) const {
            return err.msg.size() != boolean;
        }

        bool operator!() const {
            return err.msg.size() != 0;
        }
    };

    enum Token {
        Identifier = 256,
        Literal,
        Number
    };

    struct token {
        int type;
        struct {
            unsigned rows, cols;
        } loc;
        union {
            long long int64;
            int int32;
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
