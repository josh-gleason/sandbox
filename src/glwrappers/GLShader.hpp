#ifndef GLSHADER_H
#define GLSHADER_H

#include <memory>
#include <string>
#include <GL/glew.h>

class GLShader
{
public:
    GLShader();

    // if this is the last instance of the shader it is flagged for deletion
    ~GLShader();

    // shaderType: The type of shader to compile, one of the following
    //   GL_VERTEX_SHADER
    //   GL_FRAGMENT_SHADER
    //   GL_GEOMETRY_SHADER
    //   GL_TESS_CONTROL_SHADER
    //   GL_TESS_EVALUATION_SHADER
    // returns false on failure. To get error log use getCompileErrors()
    bool compileFromFile(const char* filename, GLenum shaderType);

    // get error strings with this if compileFromFile fails
    const std::string& getLastError() const;
    
protected:
    std::string m_err;

    std::shared_ptr<GLuint> m_shader;
    GLenum m_type;

    friend class GLProgram;
};

#endif // GLSHADER_H

