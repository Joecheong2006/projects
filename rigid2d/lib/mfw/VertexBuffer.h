#pragma once
#include "util.h"

namespace mfw {
    class VertexBuffer
    {
    private:
        u32 m_id;

    public:
        explicit VertexBuffer(const void* data, u32 size);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;
        void setBuffer(const void* data, u32 size) const;

    };
}

