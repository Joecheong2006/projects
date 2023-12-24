#include "draw.h"

#include "buffer.h"
#include "screen.h"

static void draws(buffer buf, int aposx, int aposy, color fg, color bg) {
    if(bg == -1)
        SET_FC_COLOR_PATTERN24(fg);
    else
        SET_COLOR_PATTERN24(fg, bg);
    GOTOXY(aposx, aposy);
    wprintf(L"%ls", buf);
    COLOR_RESET();
}

static void drawc(buffer_type buf, int aposx, int aposy, color fg, color bg) {
    if(bg == -1)
        SET_FC_COLOR_PATTERN24(fg);
    else
        SET_COLOR_PATTERN24(fg, bg);
    GOTOXY(aposx, aposy);
    wprintf(L"%lc", buf);
    COLOR_RESET();
}

void window_draw(struct window* w) {
    draws(w->buf, w->apos.x, w->apos.y, w->layout.ctx.fg, w->layout.ctx.bg);

    struct vec2i size = { GTW(w->size), GTH(w->size) };
    for(size_t i = wcslen(w->buf); i < (size_t)size.x; ++i)
        drawc(L' ', w->apos.x+i, w->apos.y, w->layout.border.fg, w->layout.border.bg);

    drawc(w->layout.border.fill[TopLeftCornerBorder], w->apos.x-1, w->apos.y-1, w->layout.border.fg, w->layout.border.bg);
    drawc(w->layout.border.fill[TopRightCornerBorder], w->apos.x+size.x, w->apos.y-1, w->layout.border.fg, w->layout.border.bg);
    drawc(w->layout.border.fill[BottomLeftCornerBorder], w->apos.x-1, w->apos.y+size.y, w->layout.border.fg, w->layout.border.bg);
    drawc(w->layout.border.fill[BottomRightCornerBorder], w->apos.x+size.x, w->apos.y+size.y, w->layout.border.fg, w->layout.border.bg);

    for(int i = 0; i < GTW(w->size); ++i)
    {
        drawc(w->layout.border.fill[HorizontalBorder], w->apos.x+i, w->apos.y-1, w->layout.border.fg, w->layout.border.bg);
        drawc(w->layout.border.fill[HorizontalBorder], w->apos.x+i, w->apos.y+size.y, w->layout.border.fg, w->layout.border.bg);
    }

    for(int i = 0; i < GTH(w->size); ++i)
    {
        drawc(w->layout.border.fill[VerticalBorder], w->apos.x-1, w->apos.y+i, w->layout.border.fg, w->layout.border.bg);
        drawc(w->layout.border.fill[VerticalBorder], w->apos.x+size.x, w->apos.y+i, w->layout.border.fg, w->layout.border.bg);
    }
}
