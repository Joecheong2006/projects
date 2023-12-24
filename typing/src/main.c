#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "screen.h"
#include <fcntl.h>
#include "draw.h"

void print_word(buffer buf, char* subfix) {
    for(int i = 0; buf[i] != ' ' ;++i)
        printf("%c", buf[i]);
    printf("%s", subfix);
}

void init()
{
    CLEAR();
    _setmode(_fileno(stdout), _O_U16TEXT);
}

int main(void)
{
    struct window screen;
    struct window w1;
    struct window w2;
    struct window w3;

    init();
    HIDE_CURSOR();

    screen_init(&screen);

    window_init(&w1, L"hello", 18, 7, 0.1, 0.2);
    window_add_sub(&screen, &w1);

    window_init(&w2, L"╰│╯╭─╮", 10, 1, 0.4, 0.4);
    window_add_sub(&screen, &w2);

    window_init(&w3, L"world", 10, 1, 0.25, 1);
    window_add_sub(&w1, &w3);

    w3.layout.ctx.bg = 0x333333;
    w1.layout.ctx.bg = -1;
    w2.layout.ctx.bg = -1;
    w1.layout.ctx.fg = 0x00ff00;

    screen_draw(&screen);

    SHOW_CURSOR();
    return 0;

    buffer ctx = read_file("book/meditations/0.0");
    buffer ctx_s = ctx;
    buffer ctx_e = ctx;
    for(int i = 0; i < 40; ++i)
    {
        ctx_s = buffer_find_next_words(ctx_e);
        ctx_e = buffer_find_next_worde(ctx_s);
        print_word(ctx_s, " ");
    }
    printf("\n");
    buffer_free(ctx);
    return 0;
}
