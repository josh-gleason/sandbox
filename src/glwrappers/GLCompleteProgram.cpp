#include "GLCompleteProgram.hpp"

#include "GLShader.hpp"

GLCompleteProgram::GLCompleteProgram() : GLProgram() {}

bool GLCompleteProgram::loadAndLink(const char* vshaderFile, const char* fshaderFile, const char* gshaderFile)
{
    GLShader vshader;
    GLShader gshader;
    GLShader fshader;

    this->init();

    // compile shaders
    if ( !vshader.compileFromFile(vshaderFile, GL_VERTEX_SHADER) )
    {
        m_err = vshader.getLastError();
        return false;
    }
    if ( gshaderFile != nullptr )
    {
        if ( !gshader.compileFromFile(gshaderFile, GL_GEOMETRY_SHADER) )
        {
            m_err = vshader.getLastError();
            return false;
        }
    }
    if ( !fshader.compileFromFile(fshaderFile, GL_FRAGMENT_SHADER) )
    {
        m_err = fshader.getLastError();
        return false;
    }

    // attach shaders
    if ( !this->attachShader(vshader) )
        return false;
    if ( gshaderFile != nullptr )
        if ( !this->attachShader(gshader) )
            return false;
    if ( !this->attachShader(fshader) )
        return false;

    // link program
    return this->link();
}

