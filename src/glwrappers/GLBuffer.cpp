#include "GLBuffer.hpp"

GLBuffer::GLBuffer() :
    m_buffers(nullptr),
    m_targets(nullptr),
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
    GLenum *targets = new GLenum[n];
    bool   *dataSet = new bool[n];
    glGenBuffers(n, buffers);

    // check results
    if ( *buffers == 0 )
    {
        delete [] buffers;
        delete [] targets;
        delete [] dataSet;
        return false;
    }
  
    // initialize arrays
    for ( int i = 0; i < n; ++i )
    {
        targets[i] = GL_NONE;
        dataSet[i] = false;
    }

    // delete old stuff
    if ( m_buffers != nullptr && m_buffers.use_count() <= 1 )
    {
        GLuint *temp_buffers = new GLuint[m_bufferCount];
        for ( GLsizei i = 0U; i < m_bufferCount; ++i )
            temp_buffers[i] = m_buffers[i];
        glDeleteBuffers(m_bufferCount, temp_buffers);
        delete [] temp_buffers;
    }
    
    // set shared array
    m_buffers = boost::shared_array<GLuint>(buffers);
    m_targets = boost::shared_array<GLenum>(targets);
    m_dataSet = boost::shared_array<bool>(dataSet);
    m_bufferCount = n;

    return true;
}

bool GLBuffer::setEmpty(GLsizeiptr sizei, GLenum usage, GLsizei idx)
{
    if ( m_buffers != nullptr && m_bufferCount > idx && m_targets[idx] != GL_NONE )
    {
        glBufferData(m_targets[idx], sizei, NULL, usage);
        m_dataSet[idx] = true;
        return true;
    }
    return true;
}

void GLBuffer::bind(GLenum target, GLsizei idx)
{
    if ( m_buffers != nullptr && m_bufferCount > idx )
    {
        glBindBuffer(target, m_buffers[idx]);
        m_targets[idx] = target;
    }
}
 
void GLBuffer::bindBase(GLuint index, GLsizei idx)
{
    if ( m_buffers != nullptr && m_bufferCount > idx && m_targets[idx] != GL_NONE )
    {
        glBindBufferBase(m_targets[idx], index, m_buffers[idx]);
    }
}

void GLBuffer::unbindBuffers(GLenum target)
{
    glBindBuffer(target, 0);
}

