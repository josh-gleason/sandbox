#include "GLBuffer.hpp"

GLBuffer::GLBuffer() :
    m_buffers(nullptr),
    m_target(GL_NONE),
    m_bufferCount(0U)
{}

GLBuffer::~GLBuffer()
{
    // if we are the last to have this, make sure to clean up
    if ( m_buffers != nullptr && m_buffers.use_count() <= 1 )
    {
        GLuint *buffers = new GLuint[m_bufferCount];
        for ( GLsizei i = 0U; i < m_bufferCount; ++i )
            buffers[i] = m_buffers[i];
        glDeleteBuffers(m_bufferCount, buffers);
        delete [] buffers;
    }
}

bool GLBuffer::generate(GLsizei n)
{
    // can't generate 0 buffers
    if ( n <= 0 )
        return false;

    // generate n buffers
    GLuint *buffers = new GLuint[n];
    glGenBuffers(n, buffers);

    // check results
    if ( *buffers == 0 )
    {
        delete [] buffers;
        return false;
    }
    
    // set shared array
    if ( m_buffers != nullptr && m_buffers.use_count() <= 1 )
    {
        GLuint *temp_buffers = new GLuint[m_bufferCount];
        for ( GLsizei i = 0U; i < m_bufferCount; ++i )
            temp_buffers[i] = m_buffers[i];
        glDeleteBuffers(m_bufferCount, temp_buffers);
        delete [] buffers;
    }
    m_buffers = boost::shared_array<GLuint>(buffers);
    m_bufferCount = n;

    return true;
}

void GLBuffer::bind(GLenum target, GLsizei idx)
{
    if ( m_buffers != nullptr && m_bufferCount > idx )
    {
        glBindBuffer(target, m_buffers[idx]);
        m_target = target;
    }
}

void GLBuffer::unbindBuffers(GLenum target)
{
    glBindBuffer(target, 0);
}

