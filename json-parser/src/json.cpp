#include "json.h"
#include <fstream>
#include <stdio.h>

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

    static ret_type<token> parse_number(std::string::const_iterator& iter) {
        (void)iter;
        return {};
    }

    ret_type<std::vector<token>> lexer::load_file(const file& f) {
        std::vector<token> result = {};
        auto& content = f.content();

        unsigned rows = 1, cols = 1;

        for (auto iter = content.begin(); iter != content.end(); ++iter) {
            switch (*iter) {
            case ':':
            case '\'':
            case '{':
            case '}':
            case '[':
            case ']': {
                result.push_back(token{ (int)*iter, { rows, cols }, {} });
                break;
            }
            case '\n': {
                ++rows;
                cols = 1;
                break;
            }
            case '"': {
                // TODO(Oct29): implement parse number
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
                // TODO(Oct28): implement parse number
                auto tok_ret = parse_number(iter);
                if (!tok_ret) {
                }
                token& tok = tok_ret.val;
                printf("\ttokens: type - %d; loc - [%d, %d]\n", tok.type, tok.loc.rows, tok.loc.cols);
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
