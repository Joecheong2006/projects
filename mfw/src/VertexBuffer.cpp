#include "VertexBuffer.h"
#include "Renderer.h"
#include "glad/gl.h"


namespace mfw {
    VertexBuffer::VertexBuffer(const void* data, u32 size, u32 usage)
        : usage(usage)
    {
        GLCALL(glGenBuffers(1, &m_id));
        setBuffer(data, size);
    }

    VertexBuffer::~VertexBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_id));
    }

    void VertexBuffer::bind() const
    {
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
    }

    void VertexBuffer::unbind() const
    {
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void VertexBuffer::setBuffer(const void* data, u32 size) const {
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
    }
}

