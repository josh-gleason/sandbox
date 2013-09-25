#include "GLShader.hpp"

#include <sstream>
#include <fstream>

GLShader::GLShader() :
    m_err(""),
    m_shader(nullptr),
    m_type(GL_SHADER_TYPE)
{}

GLShader::~GLShader()
{
    if ( m_shader != nullptr && m_shader.use_count() == 1 )
    {
        glDeleteShader(*m_shader);
    }
}

bool GLShader::compileFromFile(const char* filename, GLenum shaderType)
{
    GLuint shader;
    GLchar *shaderSource;
    GLchar *shaderLog;
    GLint shaderSourceSize;
    GLint shaderStatus;
    GLint shaderLogSize;

    // open the file
    std::ifstream fin(filename, std::ios::binary);
    if ( !fin.good() )
    {
        std::ostringstream sout;
        sout << "Unable to open file \"" << filename << "\"";
        m_err = sout.str();
        return false;
    }

    // read file
    fin.seekg( 0, std::ios::end );
    shaderSourceSize = fin.tellg();
    fin.seekg( 0, std::ios::beg );
    
    shaderSource = new GLchar[shaderSourceSize];
    fin.read( shaderSource, shaderSourceSize );
    fin.close();

    // create the shader
    shader = glCreateShader(shaderType);
    if ( shader == 0 )
    {
        m_err = "Invalid shader type";
        return false;
    }
    
    // load source
    glShaderSource(shader, 1, const_cast<const GLchar**>(&shaderSource), &shaderSourceSize);
    delete [] shaderSource;

    // compile shader
    glCompileShader(shader);
    
    // check compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatus);
    if ( shaderStatus != GL_TRUE )
    {
        std::ostringstream sout;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shaderLogSize);
        shaderLog = new GLchar[shaderLogSize];
        glGetShaderInfoLog(shader, shaderLogSize, nullptr, shaderLog);

        sout << "Failed to compile \"" << filename << '\"' << std::endl
             << shaderLog;
        m_err = sout.str();

        glDeleteShader(shader);
        return false;
    }

    // initialize the class variable
    if ( m_shader != nullptr && m_shader.use_count() <= 1 )
        glDeleteShader(*m_shader);
    m_shader = std::shared_ptr<GLuint>(new GLuint);
    *m_shader = shader;

    return true;
}

const std::string& GLShader::getLastError() const
{
    return m_err;
}

