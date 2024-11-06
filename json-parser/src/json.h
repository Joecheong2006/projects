#pragma once

#include <vector>
#include <string>

namespace json {
    enum ErrorType {
        InvalidInput = 1,
        InvalidWord,
        InvalidFormat,
        InvalidType,
    };

    struct error {
        error() = default;
        error(const std::string& msg, ErrorType type, unsigned rows = -1, unsigned cols = -1)
            : msg(msg), type(type), rows(rows), cols(cols) {}

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
            char* string;
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

    using tokens = std::vector<token>;
    ret_type<std::vector<token>> lex(const file& f);

    struct primitive;
    class json {
    private:
        primitive* pri = nullptr;

    public:
        json(primitive* pri = nullptr): pri(pri) {}

        ~json();

        json(json&& j) {
            pri = j.pri;
            j.pri = nullptr;
        }

        void operator=(json&& j);

        json(const json&) = delete;
        void operator=(const json&) = delete;
        
        primitive* operator->() const {
            return pri;
        }

        friend std::ostream& operator<<(std::ostream& os, const json& j);

    };

    struct object;
    struct string;
    struct number;
    struct object;
    struct array;
    struct boolean;
    
    struct primitive {
        virtual ~primitive() = default;

        inline virtual bool is_string() const { return false; }
        inline virtual string* get_string() { return nullptr; }

        inline virtual bool is_number() const { return false; }
        inline virtual number* get_number() { return nullptr; }
        
        inline virtual bool is_object() const { return false; }
        inline virtual object* get_object() { return nullptr; }

        inline virtual bool is_array() const { return false; }
        inline virtual array* get_array() { return nullptr; }

        inline virtual bool is_boolean() const { return false; }
        inline virtual boolean* get_boolean() { return nullptr; }

        inline virtual bool is_null() const { return false; }

        virtual void log() const = 0;
        virtual std::string dump() const = 0;

        primitive* get(int index);
        primitive* get(std::string key);
        primitive* get(const char* key);

        friend std::ostream& operator<<(std::ostream& os, primitive* pri);
    };
    ret_type<json> parse(const tokens& toks);
}
