#include "Renderer.h"
#include "glad/gl.h"

namespace mfw {
    void GLClearError()
    {
        while(glGetError() != GL_NO_ERROR);
    }

    bool GLLogCall(const char* file, i32 line, const char* func)
    {
        while(GLenum error = glGetError())
        {
            LOG_INFO("[{}:{}:{}:{}]", file, line, func, error);
            return false;
        }
        return true;
    }

}
