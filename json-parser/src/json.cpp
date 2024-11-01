#include "json.h"
#include <fstream>
#include <cstring>
#include <cmath>

namespace json {
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

    static ret_type<int> parse_literal(token& tok, std::string::const_iterator& begin) {
        int skip_len = 1;
        for (auto iter = ++begin;; ++iter, ++skip_len) {
            if (*iter == EOF) {
                return {{}, {"missing string closing braket", ErrorType::InvalidInput}};
            }
            if (*iter == '"') {
                tok.val.literal = new char[skip_len];
                const char* src = &(*begin);
                std::memcpy(tok.val.literal, src, skip_len);
                tok.val.literal[skip_len - 1] = '\0';
                begin += skip_len - 1;
                return {skip_len, {}};
            }
            // TODO(Oct31): implement character encoding
        }
    }

    inline static ret_type<int> parse_number(token& tok, std::string::const_iterator& begin) {
        int skip_len = 1;
        size_t precision_factor = 1;
    
        auto iter = begin;
        for (;; ++iter, ++skip_len) {
            if (*iter == EOF) {
                return {{}, {"failed to parse number", ErrorType::InvalidInput}};
            }

            if (precision_factor > 1) {
                precision_factor *= 10;
            }

            if (*iter == '.') {
                if (precision_factor > 1) {
                    return {{}, {"failed to parse float. There are more than one '.'", ErrorType::InvalidFormat}};
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
                    ret_type<int> ret = {{}, {"unkown exp flag ", ErrorType::InvalidFormat}};
                    ret.err.msg.push_back(*iter);
                    return ret;
                }
                ++iter, ++skip_len;
            }

            for (;; ++iter, ++skip_len) {
                if (*iter == EOF) {
                    return {{}, {"failed to parse number", ErrorType::InvalidInput}};
                }
                else if (*iter == '.') {
                    return {{}, {"exp cannot be floating point", ErrorType::InvalidFormat}};
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

    ret_type<std::vector<token>> lexer::load_file(const file& f) {
        std::vector<token> result = {};
        auto& content = f.content();

        unsigned rows = 1, cols = 1;

        for (auto iter = content.begin(); iter != content.end(); ++iter) {
            switch (*iter) {
            case 'n':
                if (std::strncmp(&*iter, "null", 4)) {
                    return {{}, {"unkown keyword", ErrorType::InvalidWord}};
                }
                result.push_back({TokenType::Null, rows, cols, {}});
                cols += 3;
                iter += 3;
                break;
            case 't': {
                if (std::strncmp(&*iter, "true", 4)) {
                    return {{}, {"unkown keyword", ErrorType::InvalidWord}};
                }
                result.push_back({TokenType::Boolean, rows, cols, {true}});
                cols += 3;
                iter += 3;
                break;
            }
            case 'f': {
                if (std::strncmp(&*iter, "false", 5)) {
                    return {{}, {"unkown keyword", ErrorType::InvalidWord}};
                }
                result.push_back({TokenType::Boolean, rows, cols, {false}});
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
                const auto ret = parse_literal(tok, iter);
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
                const auto ret = parse_number(tok, iter);
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

}
