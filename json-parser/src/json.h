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

    using tokens = std::vector<token>;
    ret_type<tokens> lex(const file& f);

    struct primitive {
        using obj = std::map<const char*, primitive*>;
        using arr = std::vector<primitive*>;
        virtual ~primitive() = default;

        inline virtual bool is_string() const { return false; }
        inline virtual ret_type<std::string> get_string() { return {{}, {"it is not a string", ErrorType::InvalidType}}; }

        inline virtual bool is_number() const { return false; }
        inline virtual ret_type<double> get_number() { return {{}, {"it is not a number", ErrorType::InvalidType}}; }
        
        inline virtual bool is_object() const { return false; }
        inline virtual ret_type<obj> get_object() { return {{}, {"it is not a object", ErrorType::InvalidType}}; }

        inline virtual bool is_array() const { return false; }
        inline virtual ret_type<arr> get_array() { return {{}, {"it is not a arrary", ErrorType::InvalidType}}; }

        inline virtual bool is_boolean() const { return false; }
        inline virtual ret_type<bool> get_boolean() { return {{}, {"it is not a boolean", ErrorType::InvalidType}}; }

        inline virtual bool is_null() const { return false; }

        inline virtual void log() const {};

        friend std::ostream& operator<<(std::ostream& os, primitive* pri) { pri->log(); return os; }

        primitive* get(int index) {
            const auto& ret = get_array();
            if (!ret) {
                return nullptr;
            }
            return ret.val[index];
        }

        primitive* get(std::string key) {
            auto&& ret = get_object();
            if (!ret) {
                return nullptr;
            }
            return ret.val[key.c_str()];
        }

        primitive* get(const char* key) {
            auto&& ret = get_object();
            if (!ret) {
                return nullptr;
            }
            return ret.val[key];
        }
        
    };

    struct string : public primitive {
        const char* val;

        string(char* val)
            : val(val) {}

        ~string() {
            delete[] val;
        }

        inline virtual bool is_string() const override { return true; }
        inline virtual ret_type<std::string> get_string() override { return {val, {}}; }
        virtual void log() const override;
    };

    struct number : public primitive {
        double val;

        number(double num)
            : val(num) {}

        inline virtual bool is_string() const override { return true; }
        inline virtual ret_type<double> get_number() override { return {val, {}}; }
        virtual void log() const override;
    };

    struct boolean : public primitive {
        bool val;

        boolean(bool val)
            : val(val) {}

        inline virtual bool is_string() const override { return true; }
        inline virtual ret_type<bool> get_boolean() override { return {val, {}}; }
        virtual void log() const override;
    };

    struct null : public primitive {
        inline virtual bool is_string() const override { return true; }
        virtual void log() const override;
    };

    struct object : public primitive {
        using type = std::map<const char*, primitive*>;
        type val;

        ~object() {
            for (const auto& e : val) {
                delete[] e.first;
                delete e.second;
            }
        }
        object(const type& obj)
            : val(obj) {}

        inline virtual bool is_string() const override { return true; }
        inline virtual ret_type<type> get_object() override { return {val, {}}; }
        virtual void log() const override;
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

        inline virtual bool is_string() const override { return true; }
        inline virtual ret_type<arr> get_array() override { return {val, {}}; }
        virtual void log() const override;
    };

    ret_type<primitive*> parse(const tokens& toks);
}
