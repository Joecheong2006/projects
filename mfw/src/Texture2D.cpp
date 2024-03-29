#include "Texture2D.h"
#include "stb_image/stb_image.h"
#include "Renderer.h"
#include "glad/gl.h"

namespace mfw {
    Texture2D::Texture2D(const char* path): Texture2D(path, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR)
    {}

    Texture2D::Texture2D(const char* path, i32 wrap, i32 min_filter, i32 mag_filer) {
        GLCALL(glGenTextures(1, &m_id));
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_id));

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));	

        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filer));

        unsigned char *data = stbi_load(path, &m_width, &m_height, &m_bpp, 0);

        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

        if(data) stbi_image_free(data);
    }

    Texture2D::~Texture2D()
    {
        GLCALL(glDeleteTextures(1, &m_id));
    }

    void Texture2D::bind(u32 slot) const
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_id));
    }

    void Texture2D::unbind() const
    {
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
}

