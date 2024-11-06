#include "../src/json.h"

#include <iostream> // std::cout

void print_tokens(std::vector<json::token> tokens) {
    for (auto& tok : tokens) {
        if (tok.type < 256) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; %c\n", tok.type, tok.rows, tok.cols, tok.type);
        }
        else if (tok.type == json::TokenType::String) {
            printf("tokens; type - %-3d; [%-3d, %-3d]; %s\n", tok.type, tok.rows, tok.cols, tok.val.string);
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

int main(void) {
    const auto loading_ret = json::file::load("compile_commands.json");
    if (!loading_ret) {
        std::cout << loading_ret.err;
        return loading_ret.err.type;
    }

    const auto& file = loading_ret.val;
    const auto lexing_ret = json::lex(file);
    if (!lexing_ret) {
        std::cout << lexing_ret.err;
        return lexing_ret.err.type;
    }

    const auto& tokens = lexing_ret.val;

    const auto& parse_ret = json::parse(tokens);
    if (!parse_ret) {
        std::cout << parse_ret.err;
        return parse_ret.err.type;
    }
    const auto& json = parse_ret.val;

    setlocale(LC_ALL, "xx_XX.UTF-8");

    std::cout << "json->dump(): " << json->dump() << "\n";
    std::cout << "\njson: " << json << std::endl;

    std::cout << "\njson->get(0)->get(\"file\"): " << json->get(0)->get("file") << std::endl;
    std::cout << "json->get(0)->get(\"text\"): " << json->get(0)->get("text") << std::endl;
}
