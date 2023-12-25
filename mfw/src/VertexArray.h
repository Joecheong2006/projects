#pragma once

#include "util.h"

namespace mfw {
    class VertexBufferLayout;
    class VertexArray
    {
    private:
        u32 m_id;

    public:
        explicit VertexArray();
        ~VertexArray();

        void applyBufferLayout(const VertexBufferLayout& layout);
        void bind() const;
        void unbind() const;
    };
}

