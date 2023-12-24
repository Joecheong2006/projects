#ifndef _SCREEN_H
#define _SCREEN_H

#include <stdint.h>
#include "buffer.h"
#include  "draw.h"

#define WINDOW_LIST_MAX 6

static int const si = sizeof(int) * 8;

#define GTW(x) (x >> si / 2)
#define GTH(x) ((x << si / 2 ) >> si / 2)

struct vec2i {
    int x;
    int y;
};

struct vec2f {
    float x;
    float y;
};

struct window_layout {
    struct {
        wchar_t fill[6];
        color fg, bg;
    } border;
    struct {
        color fg, bg;
    } ctx;
};

static const color def_fg = 0xffffff;
static const color def_bg = -1;

enum {
    VerticalBorder,
    HorizontalBorder,
    TopLeftCornerBorder,
    BottomLeftCornerBorder,
    TopRightCornerBorder,
    BottomRightCornerBorder,
};

static const struct window_layout def_layout = {
    .border.fill = L"│─╭╰╮╯",
    .border.fg = def_fg,
    .border.bg = def_bg,
    .ctx.fg = def_fg,
    .ctx.bg = def_bg,
};

struct window {
    int size;
    buffer buf;
    struct vec2f rpos;
    struct vec2i apos;
    int8_t sub_wl_size;
    struct window* sub_wl[WINDOW_LIST_MAX];
    struct window_layout layout;
};

void screen_init(struct window* s);
void screen_update(struct window* s);
void screen_draw(struct window* w);
void window_add_sub(struct window* s, struct window* w);
void window_init(struct window* w, buffer ctx, int width, int height, float rx, float ry);

#endif
