#ifndef _DRAW_H
#define _DRAW_H

#define GOTOXY(x, y) wprintf(L"\e[%d;%dH", (y), (x))
#define CLEAR() wprintf(L"\e[H\e[J")
#define HIDE_CURSOR() wprintf(L"\e[?25l")
#define SHOW_CURSOR() wprintf(L"\e[?25h")

typedef int color;

static const color white = 0xffffff;
static const color transparent = -1;

#define COLOR(r, g, b) ((r << 16) | (b << 8) | g)
#define COLOR_RESET() wprintf(L"\e[0m")
#define GET_COLOR_R(x) ((unsigned int)x >> 16)
#define GET_COLOR_G(x) (((unsigned int)x << 16) >> 24)
#define GET_COLOR_B(x) (((unsigned int)x << 24) >> 24)
#define COLORHEX(x)\
    GET_COLOR_R(x), GET_COLOR_G(x), GET_COLOR_B(x)

#define SET_COLOR_BOLD() wprintf(L"\e[1;1m")
#define SET_COLOR_REGULAR() wprintf(L"\e[0;0m")
#define SET_COLOR_PATTERN(fg, bg) wprintf(L"\e[%d;%dm", fg, bg)
#define SET_COLOR_PATTERN8(fg, bg) wprintf(L"\e[38;48;%dm\e[48;48;%dm", fg, bg)
#define SET_COLOR_PATTERN24(fc, bc) wprintf(L"\e[38;2;%d;%d;%dm\e[48;2;%d;%d;%dm", COLORHEX(fc), COLORHEX(bc))

#define SET_FC_COLOR_PATTERN24(fc) wprintf(L"\e[38;2;%d;%d;%dm", COLORHEX(fc))
#define SET_BC_COLOR_PATTERN24(bc) wprintf(L"\e[48;2;%d;%d;%dm", COLORHEX(bc))

struct window;
void window_draw(struct window* w);

#endif
