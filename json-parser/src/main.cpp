#include "json.h"

#include <iostream> // std::cout

int main(void) {
    const auto loading_ret = json::file::load("compile_commands.json");
    if (!loading_ret) {
        std::cout << loading_ret.err.msg << std::endl;
        return loading_ret.err.type;
    }

    const auto lexing_ret = json::lexer::load_file(loading_ret.val);
    const auto tokens = lexing_ret.val;

    for (auto& tok : tokens) {
        printf("tokens: type - %d; loc - [%d, %d]\n", tok.type, tok.loc.rows, tok.loc.cols);
    }
}
