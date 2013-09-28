#include "GLVertexArray.hpp"

GLVertexArray::GLVertexArray() :
    m_vao(nullptr),
    m_count(0)
{}

GLVertexArray::~GLVertexArray()
{
    removeBeforeDelete();
}

void GLVertexArray::create(GLsizei count)
{
    removeBeforeDelete();
    GLuint *vao = new GLuint[count];
    
    // create new vertex array object
    glGenVertexArrays(count, vao);
    
    // copy values to member variables
    m_count = count;
    m_vao = boost::shared_array<GLuint>(vao);
}

void GLVertexArray::bind(int idx)
{
    if ( m_vao != nullptr )
        glBindVertexArray(m_vao[idx]);
}

void GLVertexArray::unbindAll()
{
    glBindVertexArray(0);
}

void GLVertexArray::removeBeforeDelete()
{
    if ( m_vao != nullptr && m_vao.use_count() == 1 )
    {
        if ( m_count > 1 )
        {
            GLuint *vao = new GLuint[m_count];
            for ( GLsizei i = 0; i < m_count; ++i )
                vao[i] = m_vao[i];
            glDeleteVertexArrays(m_count, vao);
            delete [] vao;
        }
        else
        {
            GLuint vao = m_vao[0];
            glDeleteVertexArrays(m_count, &vao);
        }
    }
}

