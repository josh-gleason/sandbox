#ifndef GLPROGRAM_HPP
#define GLPROGRAM_HPP

#include <GL/glew.h>
#include <memory>
#include <string>
#include <map>
#include <vector>

class GLShader;

class GLProgram
{
public:
    typedef std::basic_string<GLchar> GLstring;
    
    GLProgram();

    // if this is the last instance of the program it is flagged for deletion
    ~GLProgram();

    GLProgram& operator=(const GLProgram &rhs);

    // create the shader program
    void init();

    // attach/detach shaders
    bool attachShader(const GLShader &shader);
    bool detachShader(const GLShader &shader);

    // link the program.
    // returns false on failure. To get error log use getLastError()
    bool link();

    // returns a string describing the most recent error
    const std::string& getLastError() const;

    // use the program
    void use();

    // calls glUseProgram(0)
    static void resetUsed();

    // query the shader for the uniform location
    GLint getUniformLocation(const GLstring& name);

    // query the shader for the attribute location
    GLint getAttributeLocation(const GLstring& name);

    // shouldn't really be used
    GLuint getProgramIdx() const;
protected:
    typedef std::map<GLstring,GLint> StringMap;

    std::shared_ptr<GLuint> m_program;
    std::string m_err;

    std::shared_ptr<StringMap> m_attributes;
    std::shared_ptr<StringMap> m_uniforms;
};

#endif // GLPROGRAM_HPP

