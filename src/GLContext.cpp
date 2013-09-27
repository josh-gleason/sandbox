#include <GL/glew.h>

#include "GLContext.hpp"

#include "GLShader.hpp"
#include "GLUniform.hpp"

#include <glm/glm.hpp>
#include <QTextStream>
#include <QApplication>

#include <glm/glm.hpp>

#include <iostream>

GLint locVerts;

GLContext::GLContext(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent),
    m_good(true)
{
    
}

void GLContext::reportError(const QString& error)
{
    QTextStream qerr(stderr);
    qerr << "Error: " << error << endl;
    m_good = false;

    // won't work in initializeGL because main loop has not been entered yet
    qApp->quit();
}

void GLContext::initializeGL()
{
    // initialize glew
    GLenum err = glewInit();
    if ( err != GLEW_OK )
        return reportError(QString::fromUtf8(reinterpret_cast<const char*>(glewGetErrorString(err))));
   
    // initilize shaders
    GLShader vshader;
    GLShader fshader;
    if ( !vshader.compileFromFile("./shaders/vshader.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshader.getLastError().c_str()));
    if ( !fshader.compileFromFile("./shaders/fshader.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshader.getLastError().c_str()));

    // initialize program
    m_glProgram.init(); 
    if ( !m_glProgram.attachShader(vshader) )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));
    if ( !m_glProgram.attachShader(fshader) )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));

    // link program
    if ( !m_glProgram.link() )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));

    // initialize triangle
    m_triangle.init(m_glProgram, "v_position", "v_color");
}

void GLContext::paintGL()
{
    m_triangle.draw();
}

bool GLContext::good() const
{
    return m_good;
}

