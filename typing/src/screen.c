#include "screen.h"
#include "draw.h"
#include <assert.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <ioctl.h>
#endif

struct vec2i get_terminal_size() {
    struct vec2i size;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    size.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
#endif
    return size;
}

void screen_init(struct window* s) {
    struct vec2i tsize = get_terminal_size();
    window_init(s, NULL, tsize.x, tsize.y, 0, 0);
}

void window_init(struct window* w, buffer ctx, int width, int height, float rx, float ry) {
    w->sub_wl_size = 0;
    w->size = ((width << si / 2) | height);
    w->apos.x = 0;
    w->apos.y = 0;
    w->rpos.x = rx;
    w->rpos.y = ry;
    w->layout = def_layout;
    w->buf = ctx;
}

void screen_update(struct window* s) {
    struct vec2i tsize = get_terminal_size();
    s->size = ((tsize.x << si / 2) | tsize.y);
}

void screen_draw(struct window* w)
{
    for(int i = 0; i < w->sub_wl_size; ++i)
    {
        window_draw(w->sub_wl[i]);
        screen_draw(w->sub_wl[i]);
    }
}

void window_add_sub(struct window* s, struct window* w) {
    assert(s->sub_wl_size != WINDOW_LIST_MAX);
    s->sub_wl[s->sub_wl_size] = w;
    w->apos.x = (int)(w->rpos.x * GTW(s->size) + s->apos.x);
    w->apos.y = (int)(w->rpos.y * GTH(s->size) + s->apos.y);
    ++s->sub_wl_size;
}

