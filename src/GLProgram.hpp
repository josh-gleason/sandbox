#ifndef GLPROGRAM_HPP
#define GLPROGRAM_HPP

#include <memory>
#include <string>
#include <GL/glew.h>

class GLShader;

class GLProgram
{
public:
    GLProgram();

    // if this is the last instance of the program it is flagged for deletion
    ~GLProgram();

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

protected:
    std::shared_ptr<GLuint> m_program;
    std::string m_err;
};

#endif // GLPROGRAM_HPP

