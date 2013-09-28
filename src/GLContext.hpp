#ifndef GLCONTEXT_HPP
#define GLCONTEXT_HPP

// need to be included first
#include "GLProgram.hpp"
#include "GLBuffer.hpp"
#include "GLUniform.hpp"
#include "Triangle.hpp"

#include <QGLWidget>

class GLContext : public QGLWidget
{
    Q_OBJECT
public:
    GLContext(QWidget *parent = nullptr);

    // check if initializeGL succeeded
    bool good() const;
protected:
    // exits the program
    void reportError(const QString& error);

    void initializeGL();
    void paintGL();

    // set in initializeGL if failure occurs
    bool        m_good;

    GLProgram              m_glProgram;
    GLBuffer               m_glVertexPosBuffer;
    GLBuffer               m_glVertexIdxBuffer;
    std::vector<GLUniform> m_glUniforms;
    Triangle               m_triangle;
    Triangle               m_triangle2;
};

#endif // GLCONTEXT_HPP

