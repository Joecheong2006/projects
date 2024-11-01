#include "json.h"

#include <iostream> // std::cout

int main(void) {
    const auto loading_ret = json::file::load("compile_commands.json");
    if (!loading_ret) {
        std::cout << loading_ret.err.msg << std::endl;
        return loading_ret.err.type;
    }

    const auto lexing_ret = json::lexer::load_file(loading_ret.val);
    if (!lexing_ret) {
        std::cout << lexing_ret.err.msg << std::endl;
        return lexing_ret.err.type;
    }

    const auto tokens = lexing_ret.val;

    for (auto& tok : tokens) {
        if (tok.type < 256) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; %c\n", tok.type, tok.rows, tok.cols, tok.type);
        }
        else if (tok.type == json::TokenType::String) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; %s\n", tok.type, tok.rows, tok.cols, tok.val.literal);
        }
        else if (tok.type == json::TokenType::Number) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; %g\n", tok.type, tok.rows, tok.cols, tok.val.number);
        }
        else if (tok.type == json::TokenType::Boolean) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; %s\n", tok.type, tok.rows, tok.cols, tok.val.boolean ? "true" : "false");
        }
        else if (tok.type == json::TokenType::Null) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; null\n", tok.type, tok.rows, tok.cols);
        }
    }
}
