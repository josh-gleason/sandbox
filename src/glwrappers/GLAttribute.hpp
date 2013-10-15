#ifndef GLATTRIBUTE_HPP
#define GLATTRIBUTE_HPP

#include <GL/glew.h>

#include "GLProgram.hpp"

class GLAttribute
{
public:
    GLAttribute(GLint pos = -1) : m_attribute(pos)
    {}

    ~GLAttribute()
    {}

    bool init(GLProgram program, const std::basic_string<GLchar>& name)
    {
        m_attribute = program.getAttributeLocation(name);
        return m_attribute != -1;
    }

    void enable()
    {
        glEnableVertexAttribArray(m_attribute);
    }
    void disable()
    {
        glDisableVertexAttribArray(m_attribute);
    }

    void loadBufferData(int count, unsigned long step, unsigned long offset = 0, GLenum type = GL_FLOAT, GLboolean normalize = GL_FALSE)
    {
        glVertexAttribPointer(m_attribute, count, type, normalize, step, reinterpret_cast<void*>(offset));
    }

    GLint getAttributeIdx() const
    {
        return m_attribute;
    }

    void tellLocation(GLint loc)
    {
        m_attribute = loc;
    }
protected:
    GLint  m_attribute;
};

#endif // GLATTRIBUTE_HPP

