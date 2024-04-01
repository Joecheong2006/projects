#pragma once
#include "util.h"

namespace mfw {
    class VertexBuffer
    {
    private:
        u32 m_id;
        u32 usage;

    public:
        explicit VertexBuffer(const void* data, u32 size, u32 usage);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;
        void setBuffer(const void* data, u32 size) const;

    };
}

