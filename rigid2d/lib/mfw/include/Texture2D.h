#pragma once

#include "util.h"

namespace mfw {
    class Texture2D
    {
    private:
        i32 m_width, m_height, m_bpp;
        u32 m_id;

    public:
        Texture2D(const char* path);
        Texture2D(const char* path, i32 wrap, i32 min_filter, i32 mag_filer);
        ~Texture2D();

        void bind(u32 slot = 0) const;
        void unbind() const;

        inline i32 width() { return m_width; }
        inline i32 height() { return m_height; }
        inline i32 bpp() { return m_bpp; }

    };
}

