#include "GLProgram.hpp"

#include "GLShader.hpp"

#include <sstream>

#define CHECK_INIT if ( m_program == nullptr ) { m_err = "Program not initialized"; return false; }

GLProgram::GLProgram() :
    m_program(nullptr),
    m_err("")
{}

GLProgram::~GLProgram()
{
    if ( m_program != nullptr && m_program.use_count() == 1 )
        glDeleteProgram(*m_program);
}

void GLProgram::init()
{
    if ( m_program != nullptr && m_program.use_count() == 1 )
        glDeleteProgram(*m_program);

    m_program = std::shared_ptr<GLuint>(new GLuint);
    *m_program = glCreateProgram();
}

bool GLProgram::attachShader(const GLShader &shader)
{
    CHECK_INIT

    GLenum error;

    // check if shader is initialized
    if ( shader.m_shader == nullptr )
    {
        m_err = "Failed to attach shader to program : Shader not initialized";
        return false;
    }

    glGetError();   // clear previous errors
    
    // attach shader
    glAttachShader(*m_program, *(shader.m_shader));

    // check for success
    error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        m_err = "Failed to attach shader to program : "
                "It is likely that the shader was already attached to the program";
        return false;
    }

    return true;
}

bool GLProgram::detachShader(const GLShader &shader)
{
    CHECK_INIT
    
    GLenum error;

    // check if shader is initialized
    if ( shader.m_shader == nullptr )
    {
        m_err = "Failed to detach shader from program : Shader not initialized";
        return false;
    }

    glGetError();   // clear previous errors

    // detach shader
    glDetachShader(*m_program, *(shader.m_shader));
    error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        m_err = "Failed to detach shader from program : "
                "It is likely that the shader was never attached to the program";
        return false;
    }

    return true;
}

bool GLProgram::link()
{
    CHECK_INIT

    GLint linkStatus;
    GLint linkLogSize;
    GLchar *linkLog;

    // link program
    glLinkProgram(*m_program);

    // check status
    glGetProgramiv(*m_program, GL_LINK_STATUS, &linkStatus);
    if ( linkStatus != GL_TRUE )
    {
        std::ostringstream sout;

        glGetProgramiv(*m_program, GL_INFO_LOG_LENGTH, &linkLogSize);
        linkLog = new GLchar[linkLogSize];
        glGetProgramInfoLog(*m_program, linkLogSize, nullptr, linkLog);
        
        sout << "Failed to link program : " << std::endl
             << linkLog;
        m_err = sout.str();

        delete [] linkLog;
        return false;
    }

    return true;
}

const std::string& GLProgram::getLastError() const
{
    return m_err;
}

void GLProgram::use()
{
    glUseProgram(m_program == nullptr ? 0 : *m_program);
}

void GLProgram::resetUsed()
{
    glUseProgram(0);
}

