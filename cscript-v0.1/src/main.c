#include <string.h>
#include "lexer.h"
#include "container/memallocate.h"

#include "command.h"

// TODO(Aug17): create scope and object struct 

// <digit>      ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
// <letter>     ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
// <hex>        ::= <digt> | [a-fA-F]
// <binary>     ::= "0" | "1"
// <intpart>    ::= <digit> {<digit>}
// <base2>      ::= "0b" <binary> {<binary>}
// <base16      ::= "0x" <hex> {<hex>}
// <int>        ::= ["+" | "-"] <intpart> | <base2>
// <float>      ::= ["+" | "-"] <intpart> | "" "." <intpart>
// <end>        ::= "\n" | ";"
// <identifer>  ::= <letter> | "_" {(<letter> | <digit> | "_")}
// <literal>    ::= <int> | <float> | char | string
// <term>       ::= <literal> | <identifier> | "(" <expr> ")" | <funcall> | "-" <term> | "+" <term>
// <expr>       ::= <term> {<operator> <expr>}
// <operator>   ::= "-" | "+" | "*" | "/"
// <params>     ::= <identifier> {"," <identifier>}
// <funcparams> ::= [<params>]
// <funcdef>    ::= "fun" <identifier> "(" <funcparams> ")" "end"
// <funcall>    ::= <identifier> "(" <funcparams> ")"
// <assign>     ::= <identifer> "=" <expr>
// <vardecl>    ::= "var" <assign>
// <statement>  ::= <vardecl> | <funcall> | <assign> <end>
// <if>         ::= "if" <expr> do {<statement>} ["end"]
// <elif>       ::= "elif" <expr> do {<statement>} ["end"]
// <else>       ::= "else" {<statement>} "end"
// <while>      ::= "while" <expr> do {<statement>} "end"

#include "parser.h"

void print_ast_tree(ast_node* node) {
    if (node == NULL)
        return;
    print_ast_tree(node->lhs);
    print_ast_tree(node->rhs);
    printf("type %d ", node->type);
}

int main(void) {
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3*3";
    const char text[] = "var a=(2+4*(3/(.2*10))+3-1-1)*1.1+(0.5+.5) + (.5-0.3-0.2)\n"
                        "var cat = 1-1.0-1--3*3";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    parser_init(&par, generate_tokens(&lex));

    for_vector(par.tokens, i, 0) {
        char buf[100];
        sprintf(buf, "%d:%d:%d ", par.tokens[i].line, par.tokens[i].count, par.tokens[i].type);
        printf("%s %*c", buf, 14 - (i32)strlen(buf), ' ');
        if (par.tokens[i].type == TokenTypeLiteralInt32) {
            printf("i32: %d\n", par.tokens[i].val.int32);
        }
        else if (par.tokens[i].type == TokenTypeLiteralFloat32) {
            printf("f32: %g\n", par.tokens[i].val.float32);
        }
        else if (par.tokens[i].type == TokenTypeIdentifier) {
            printf("id:  %s len: %d\n", par.tokens[i].val.string, (i32)strlen(par.tokens[i].val.string));
        }
        else {
            if (par.tokens[i].type < 256)
                printf("sym: '%c' asc: %d\n", par.tokens[i].type == '\n' ? ' ' : par.tokens[i].type, par.tokens[i].type);
            else
                printf("key: %s\n", TokenTypeString[par.tokens[i].type - 256]);
        }
    }

    vector(ast_node*) ins = parser_parse(&par);
    if (ins) {
        for_vector(ins, i, 0) {
            // print_ast_tree(ins[i]);
            // putchar('\n');
            command* cmd = ins[i]->gen_command(ins[i]);
            if (cmd->exec(NULL, cmd)) {
                printf("%s = %g\n", cmd->arg1->name, cmd->arg2->data->float32);
                free_string(cmd->arg1->name);
            }
            command_free(cmd);
        }
        for_vector(ins, i, 0) {
            ast_tree_free(ins[i]);
        }
        free_vector(ins);
    }

    for_vector(par.errors, i, 0) {
        printf("%d:%d %s\n", par.errors[i].tok->line, par.errors[i].tok->count, par.errors[i].msg);
    }

    parser_free(&par);
    // free_vector(lex.ctx);

    printf("leak count = %d\n", check_memory_leak());
    return 0;
}
