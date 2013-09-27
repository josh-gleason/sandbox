#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <boost/shared_array.hpp>
#include <GL/glew.h>

#include <iostream>

class GLBuffer
{
public:
    GLBuffer();

    // if this is the GLBuffer holding an instance of the m_buffers pointer
    // then the buffer is deleted
    ~GLBuffer();

    // Generate new buffers, the size of n. This must be called before
    //   binding.
    bool generate(GLsizei n = 1);

    // Bind a buffer object
    // idx: The index of the buffer to bind
    // target: One of the following
    //   GL_ARRAY_BUFFER
    //   GL_ATOMIC_COUNTER_BUFFER
    //   GL_COPY_READ_BUFFER_BINDING
    //   GL_COPY_WRITE_BUFFER_BINDING
    //   GL_DRAW_INDIRECT_BUFFER
    //   GL_ELEMENT_ARRAY_BUFFER
    //   GL_PIXEL_PACK_BUFFER
    //   GL_PIXEL_UNPACK_BUFFER
    //   GL_TEXTURE_BUFFER
    //   GL_TRANSFORM_FEEDBACK_BUFFER
    //   GL_UNIFORM_BUFFER
    void bind(GLenum target, GLsizei idx = 0);
  
    // set the data
    template <typename T>
    bool setData(const T* data, GLsizei size = 1, GLenum usage = GL_STATIC_DRAW, GLsizei idx = 0)
    {
        if ( m_buffers != nullptr && m_bufferCount > idx && m_target != GL_NONE )
        {
            glBufferData(m_target, sizeof(T)*size, reinterpret_cast<const void*>(data), usage);
        }

        // not initialized or not bound
        return false;
    }

    // unbind buffers
    // target: One of the GLenum buffers (see GLBuffer::bind)
    static void unbindBuffers(GLenum target);
protected:
    boost::shared_array<GLuint> m_buffers;
    GLenum                      m_target;
    GLsizei m_bufferCount;
};

#endif // GLBUFFER_H

