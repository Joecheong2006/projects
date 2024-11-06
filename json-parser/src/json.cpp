#include "json.h"
#include <fstream>
#include <cstring>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <sstream>

namespace json {
    std::ostream& operator<<(std::ostream& os, const error& err) {
        printf("[ERROR][%d, %d][%s]\n", err.rows, err.cols, err.msg.c_str());
        return os;
    }

    json::~json() {
        if (pri) {
            delete pri;
        }
    }

    void json::operator=(json&& j) {
        if (pri) {
            delete pri;
        }
        pri = j.pri;
        j.pri = nullptr;
    }

    std::ostream& operator<<(std::ostream& os, primitive* pri) {
        if (!pri) {
            std::cout << "nullptr";
            return os;
        }
        pri->log();
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const json& j) {
        if (!j.pri) {
            std::cout << "nullptr";
            return os;
        }
        j.pri->log();
        return os;
    }

    primitive* primitive::get(int index) {
        const auto& ret = get_array();
        if (!ret) {
            return nullptr;
        }
        return ret->val[index];
    }

    primitive* primitive::get(std::string key) {
        auto ret = get_object();
        if (!ret) {
            return nullptr;
        }
        return ret->val[key.c_str()];
    }

    primitive* primitive::get(const char* key) {
        auto ret = get_object();
        if (!ret) {
            return nullptr;
        }
        return ret->val[key];
    }

    ret_type<file> file::load(const std::string& path) {
        ret_type<file> ret = {};
        std::ifstream stream(path);
        if (!stream.is_open()) {
            ret.err.msg = "invalid to open/find file";
            ret.err.type = ErrorType::InvalidInput;
            return ret;
        }

        ret.val.m_content.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());

        stream.close();
        return ret;
    }

    /*
     string = quotation-mark *char quotation-mark
    
     char = unescaped /
         escape (
             %x22 /          ; "    quotation mark  U+0022
             %x5C /          ; \    reverse solidus U+005C
             %x2F /          ; /    solidus         U+002F
             %x62 /          ; b    backspace       U+0008
             %x66 /          ; f    form feed       U+000C
             %x6E /          ; n    line feed       U+000A
             %x72 /          ; r    carriage return U+000D
             %x74 /          ; t    tab             U+0009
             %x75 4HEXDIG )  ; uXXXX                U+XXXX
    
     escape = %x5C              ; \
    
     quotation-mark = %x22      ; "
    
     unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
    */

    static std::string itoutf8_encoding(int unicode) {
        std::string str;
        if (unicode <= 0x7f) {
            char b1 = (char)(unicode & 0x7F);
            str.push_back(b1);
        }
        else if (unicode <= 0x7ff) {
            char b1 = 0xC0 | (char)((unicode & 0x7C0) >> 6);
            char b2 = 0x80 | (char)(unicode & 0x3f);
            str.push_back(b1);
            str.push_back(b2);
        }
        else if (unicode <= 0xffff) {
            char b1 = 0xE0 | (char)((unicode & 0xF000) >> 12);
            char b2 = 0x80 | (char)((unicode & 0xFC0) >> 6);
            char b3 = 0x80 | (char)(unicode & 0x3F);
            str.push_back(b1);
            str.push_back(b2);
            str.push_back(b3);
        }
        return str;
    }

    static ret_type<int> parse_string(token& tok, const std::string& content, std::string::const_iterator& begin) {
        int skip_len = 1;
        std::string str;
        for (auto iter = ++begin;; ++iter, ++skip_len) {
            if (iter == content.end()) {
                return {skip_len, {"missing string closing braket", ErrorType::InvalidInput, tok.rows, tok.cols + skip_len - 2}};
            }
            if (*iter == '"') {
                tok.val.string = new char[str.size() + 1];
                std::memcpy(tok.val.string, str.c_str(), str.size() + 1);
                begin += skip_len - 1;
                return {skip_len, {}};
            }
            if (*iter == '\\') {
                ++iter, ++skip_len;
                switch(*iter) {
                case 0x22: str.push_back('"'); continue;
                case 0x5C: str.push_back('\\'); continue;
                case 0x2F: str.push_back('/'); continue;
                case 0x62: str.push_back('\b'); continue;
                case 0x66: str.push_back('\f'); continue;
                case 0x6E: str.push_back('\n'); continue;
                case 0x72: str.push_back('\r'); continue;
                case 0x74: str.push_back('\t'); continue;
                case 0x75: {
                    int hex = 0x0000;
                    int i;
                    for (i = 0; i < 4; ++i) {
                        ++iter;
                        if (isdigit(*iter)) {
                            hex = hex * 16 + *iter - '0';
                            continue;
                        }

                        switch (*iter) {
                        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                            hex = hex * 16 + *iter - 'a' + 10;
                            break;
                        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                            hex = hex * 16 + *iter - 'A' + 10;
                            break;
                        default:
                            return {skip_len, {"\\u must follow exactly 4 hex number", ErrorType::InvalidInput, tok.rows, tok.cols + skip_len - 2}};
                        }
                    }

                    str += itoutf8_encoding(hex);
                    skip_len += 4;
                    continue;
                }
                default:
                    return {skip_len, {"unkown special character.", ErrorType::InvalidInput, tok.rows, tok.cols + skip_len - 2}};
                }
            }
            str.push_back(*iter);

            // TODO(Oct31): implement character encoding
        }
    }

    static ret_type<int> parse_number(token& tok, const std::string& content, std::string::const_iterator& begin) {
        int skip_len = 1;
        size_t precision_factor = 1;
    
        auto iter = begin;
        for (;; ++iter, ++skip_len) {
            if (iter == content.end()) {
                return {skip_len, {"failed to parse number", ErrorType::InvalidInput, tok.rows, tok.cols + skip_len - 2}};
            }

            if (precision_factor > 1) {
                precision_factor *= 10;
            }

            if (*iter == '.') {
                if (precision_factor > 1) {
                    return {skip_len, {"failed to parse float. There are more than one '.'", ErrorType::InvalidFormat, tok.rows, tok.cols}};
                }
                precision_factor *= 10;
                ++iter, ++skip_len;
            }

            if (isdigit(*iter)) {
                tok.val.number = tok.val.number * 10.0 + (*iter - '0');
                continue;
            }
            if (precision_factor > 1) {
                tok.val.number *= 10.0 / precision_factor;
            }
            break;
        }

        if (*iter == 'e') {
            int exp = 0;

            ++iter, ++skip_len;
            bool minus_flag = false;
            if (!isdigit(*iter)) {
                if (*iter == '-') {
                    minus_flag = true;
                }
                else if (*iter == '+') {
                    minus_flag = false;
                }
                else {
                    ret_type<int> ret = {skip_len - 1, {"unkown exp flag ", ErrorType::InvalidFormat, tok.rows, tok.cols}};
                    ret.err.msg.push_back(*iter);
                    return ret;
                }
                ++iter, ++skip_len;
            }

            for (;; ++iter, ++skip_len) {
                if (iter == content.end()) {
                    return {skip_len, {"failed to parse number", ErrorType::InvalidInput, tok.rows, tok.cols + skip_len - 2}};
                }
                else if (*iter == '.') {
                    return {skip_len, {"exp cannot be floating point", ErrorType::InvalidFormat, tok.rows, tok.cols}};
                }

                if (isdigit(*iter)) {
                    exp = exp * 10 + *iter - '0';
                    continue;
                }
                tok.val.number *= std::pow(10, minus_flag ? -exp : exp);
                break;
            }
        }
        begin += skip_len - 2;
        return {skip_len - 2, {}};
    }

    ret_type<std::vector<token>> lex(const file& f) {
        std::vector<token> result = {};
        auto& content = f.content();

        unsigned rows = 1, cols = 1;

        for (auto iter = content.begin(); iter != content.end(); ++iter) {
            switch (*iter) {
            case 'n':
                if (std::strncmp(&*iter, "null", 4)) {
                    return {{}, {"unkown keyword", ErrorType::InvalidWord, rows, cols}};
                }
                result.push_back({TokenType::Null, rows, cols, {}});
                cols += 3;
                iter += 3;
                break;
            case 't': {
                if (std::strncmp(&*iter, "true", 4)) {
                    return {{}, {"unkown keyword", ErrorType::InvalidWord, rows, cols}};
                }
                token tok = {TokenType::Boolean, rows, cols, {}};
                tok.val.boolean = true;
                result.push_back(tok);
                cols += 3;
                iter += 3;
                break;
            }
            case 'f': {
                if (std::strncmp(&*iter, "false", 5)) {
                    return {{}, {"unkown keyword", ErrorType::InvalidWord, rows, cols}};
                }
                token tok = {TokenType::Boolean, rows, cols, {}};
                tok.val.boolean = false;
                result.push_back(tok);
                cols += 4;
                iter += 4;
                break;
            }
            case ':':
            case ',':
            case '\'':
            case '{':
            case '}':
            case '[':
            case ']': {
                result.push_back(token{ (int)*iter, rows, cols, {} });
                break;
            }
            case '\n': {
                ++rows;
                cols = 0;
                break;
            }
            case '"': {
                token tok = {TokenType::String, rows, cols, {}};
                const auto ret = parse_string(tok, content, iter);
                if (!ret) {
                    return {{}, ret.err};
                }
                result.push_back(tok);
                cols += ret.val;
                break;
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                token tok = {TokenType::Number, rows, cols, {}};
                auto ret = parse_number(tok, content, iter);
                if (!ret) {
                    return {{}, ret.err};
                }
                result.push_back(tok);
                cols += ret.val;
                break;
            }
            default:
                break;
            }
            ++cols;
        }
        return {result, {}};
    }

    void string::log() const {
        std::cout << val;
    }

    std::string string::dump() const {
        std::string ret;
        for (int i = 0; val[i] != '\0'; ++i) {
            unsigned short c = val[i];
            if (c < 128) {
                switch (c) {
                    case '"': ret += "\\\""; continue;
                    case '\\': ret += "\\\\"; continue;
                    case '/':  ret += "\\/"; continue;
                    case '\b': ret += "\\b"; continue;
                    case '\f': ret += "\\f"; continue;
                    case '\n': ret += "\\n"; continue;
                    case '\r': ret += "\\r"; continue;
                    case '\t': ret += "\\t"; continue;
                    default:
                       ret.push_back(c);
                       continue;
                }
            }
            else if ((c & 0xE0) == 0xC0) {
                char b1 = val[i] & 0x1F;
                char b2 = val[++i] & 0x3F;
                short code = b1 << 6 | b2;
                std::stringstream ss;
                ss << "\\u" << std::uppercase << std::hex << code;
                ret += ss.str();
            }
            else if ((c & 0xF0) == 0xE0) {
                char b1 = val[i] & 0xF;
                char b2 = val[++i] & 0x3F;
                char b3 = val[++i] & 0x3F;
                short code = b1 << 12 | b2 << 6 | b3;
                std::stringstream ss;
                ss << "\\u" << std::uppercase << std::hex << code;
                ret += ss.str();
            }
        }
        return '\"' + ret + '\"';
    }

    void number::log() const {
        std::cout << val;
    }

    void object::log() const {
        std::cout << '{';
        for (auto iter = val.begin(); iter != val.end();) {
            std::cout << iter->first << ": " << iter->second;
            if (++iter != val.end()) {
                std::cout << ", ";
                continue;
            }
            break;
        }
        std::cout << '}';
    }

    void array::log() const {
        std::cout << '[';
        if (val.size() == 0) {
            std::cout << ']';
            return;
        }
        for (int i = 0; i < (int)val.size() - 1; ++i) {
            std::cout << val[i] << ", ";
        }
        std::cout << val.back();
        std::cout << ']';
    }

    void boolean::log() const {
        std::cout << (val ? "true" : "false");
    }

    void null::log() const {
        std::cout << "null";
    }

    static ret_type<primitive*> parse_impl(const tokens& toks, tokens::const_iterator& iter);

    static ret_type<primitive*> parse_array(const tokens& toks, tokens::const_iterator& iter) {
        array::type arr;

        do {
            if (iter == toks.end()) {
                return {nullptr, {"missing closing bracket ] for array", ErrorType::InvalidFormat, (iter - 1)->rows, (iter - 1)->cols}};
            }

            if (iter->type == ']') {
                return {new array(arr), {}};
            }
            else if (iter->type == ',') {
                ++iter;
            }

            const auto& ret = parse_impl(toks, iter);
            if (!ret) {
                return {nullptr, ret.err};
            }
            ++iter;

            arr.push_back(ret.val);
        } while (iter != toks.end());
        return {};
    }

    static ret_type<primitive*> parse_object(const tokens& toks, tokens::const_iterator& iter) {
        object::type obj;

        do {
            if (iter == toks.end()) {
                return {nullptr, {"missing closing bracket } for object", ErrorType::InvalidFormat, (iter - 1)->rows, (iter - 1)->cols}};
            }
            if (iter->type == '}') {
                return {new object(obj), {}};
            }
            else if (iter->type == ',') {
                ++iter;
            }

            if (iter->type != TokenType::String) {
                return {nullptr, {"key in object must be string", ErrorType::InvalidFormat, (iter - 1)->rows, (iter - 1)->cols}};
            }
            const auto& key = iter->val.string;
            ++iter;

            if (iter->type != ':') {
                return {nullptr, {"missing : for key mapping", ErrorType::InvalidFormat, (iter - 1)->rows, (iter - 1)->cols}};
            }
            ++iter;

            const auto& value_ret = parse_impl(toks, iter);
            if (!value_ret) {
                return {nullptr, value_ret.err};
            }
            ++iter;

            obj[key] = value_ret.val;
            delete[] key;
        } while (iter != toks.end());
        return {};
    }

    static ret_type<primitive*> parse_impl(const tokens& toks, tokens::const_iterator& iter) {
        switch (iter->type) {
        case String: {
            return {new string(iter->val.string), {}};
        }
        case Number: {
            return {new number(iter->val.number), {}};
        }
        case Boolean: {
            return {new boolean(iter->val.boolean), {}};
        }
        case Null: {
            return {new null(), {}};
        }
        case '{': {
            const auto& ret = parse_object(toks, ++iter);
            if (!ret) {
                return {nullptr, ret.err};
            }
            return {ret.val, {}};
        }
        case '}': {
            return {nullptr, {"missing element in object", ErrorType::InvalidFormat, iter->rows, iter->cols}};
        }
        case '[': {
            const auto& ret = parse_array(toks, ++iter);
            if (!ret) {
                return {nullptr, ret.err};
            }
            return {ret.val, {}};
        }
        case ']': {
            return {nullptr, {"missing element in array", ErrorType::InvalidFormat, iter->rows, iter->cols}};
        }
        default:
            break;
        }
        return {};
    }

    ret_type<json> parse(const tokens& toks) {
        tokens::const_iterator iter = toks.begin();
        const auto ret = parse_impl(toks, iter);
        if (!ret) {
            return {{}, ret.err};
        }
        return {{ret.val}, {}};
    }
}
