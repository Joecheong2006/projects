#pragma once

#include <vector>
#include <string>
#include <map>

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
        using obj = std::map<const char*, json>;
        using arr = std::vector<primitive*>;
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

    struct string : public primitive {
        const char* val;

        string(char* val)
            : val(val) {}

        ~string() {
            delete[] val;
        }

        virtual bool is_string() const override { return true; }
        virtual string* get_string() override { return this; }
        virtual void log() const override;

        virtual std::string dump() const override {
            std::string ret;
            for (int i = 0; val[i] != '\0'; ++i) {
                switch (val[i]) {
                case '"': ret += "\\\""; continue;
                case '\\': ret += "\\\\"; continue;
                case '/':  ret += "\\/"; continue;
                case '\b': ret += "\\b"; continue;
                case '\f': ret += "\\f"; continue;
                case '\n': ret += "\\n"; continue;
                case '\r': ret += "\\r"; continue;
                case '\t': ret += "\\t"; continue;
                default:
                    ret.push_back(val[i]);
                    continue;
                }
            }
            return '\"' + ret + '\"';
        }
    };

    struct number : public primitive {
        double val;

        number(double num)
            : val(num) {}

        virtual bool is_string() const override { return true; }
        virtual number* get_number() override { return this; }
        virtual void log() const override;

        virtual std::string dump() const override {
            return std::to_string(val);
        }
    };

    struct boolean : public primitive {
        bool val;

        boolean(bool val)
            : val(val) {}

        virtual bool is_boolean() const override { return true; }
        virtual boolean* get_boolean() override { return this; }
        virtual void log() const override;

        virtual std::string dump() const override {
            return val ? "true" : "false";
        }
    };

    struct null : public primitive {
        virtual bool is_null() const override { return true; }
        virtual void log() const override;

        virtual std::string dump() const override {
            return "null";
        }
    };

    struct object : public primitive {
        using type = std::map<std::string, primitive*>;
        type val;

        ~object() {
            for (const auto& e : val) {
                delete e.second;
            }
        }
        object(const type& obj)
            : val(obj) {}

        virtual bool is_object() const override { return true; }
        virtual object* get_object() override { return this; }
        virtual void log() const override;

        virtual std::string dump() const override {
            std::string ret;
            for (auto iter = val.begin(); iter != val.end();) {
                ret += '"' + iter->first + "\":" + iter->second->dump();
                if (++iter != val.end()) {
                    ret += ",";
                    continue;
                }
                break;
            }
            return '{' + ret + '}';
        }
    };

    struct array : public primitive {
        using type = std::vector<primitive*>;
        type val;

        ~array() {
            for (const auto& e : val) {
                delete e;
            }
        }
        array(const type& arr)
            : val(arr) {}

        virtual bool is_array() const override { return true; }
        virtual array* get_array() override { return this; }
        virtual void log() const override;

        virtual std::string dump() const override {
            if (val.size() == 0) {
                return "[]";
            }

            std::string ret;
            for (int i = 0; i < (int)val.size() - 1; ++i) {
                ret += val[i]->dump() + ',';
            }
            ret += val.back()->dump() + ']';
            return ret;
        }
    };

    ret_type<json> parse(const tokens& toks);
}
