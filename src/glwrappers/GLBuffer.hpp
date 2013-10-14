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
 
    // must be called after bind in order to get correct target
    void bindBase(GLuint index, GLsizei idx = 0);

    // size in bytes
    bool setEmpty(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW, GLsizei idx = 0);

    template <typename T>
    bool setSubData(const T* data, GLintptr offset = 0, GLsizei size = 1, GLsizei idx = 0)
    {
        if ( m_buffers != nullptr && m_bufferCount > idx && m_dataSet[idx] )
        {
            glBufferSubData(m_targets[idx], offset, sizeof(T)*size, reinterpret_cast<const void*>(data)); 
            return true;
        }
        return false;
    }

    // set the data
    template <typename T>
    bool setData(const T* data, GLsizei size = 1, GLenum usage = GL_STATIC_DRAW, GLsizei idx = 0)
    {
        if ( m_buffers != nullptr && m_bufferCount > idx && m_targets[idx] != GL_NONE )
        {
            glBufferData(m_targets[idx], sizeof(T)*size, reinterpret_cast<const void*>(data), usage);
            m_dataSet[idx] = true;
            return true;
        }

        // not initialized or not bound
        return false;
    }

    // unbind buffers
    // target: One of the GLenum buffers (see GLBuffer::bind)
    static void unbindBuffers(GLenum target);

    GLuint getBufferIdx(GLsizei idx = 0)
    {
        if ( m_buffers != nullptr && m_bufferCount > idx )
            return m_buffers[idx];
        else return 0;
    }
protected:
    boost::shared_array<GLuint> m_buffers;
    boost::shared_array<GLenum> m_targets;
    boost::shared_array<bool>   m_dataSet;
    GLsizei m_bufferCount;
};

#endif // GLBUFFER_H

