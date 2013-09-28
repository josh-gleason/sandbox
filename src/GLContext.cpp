#include <GL/glew.h>

#include "GLContext.hpp"

#include "GLShader.hpp"
#include "GLUniform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QTextStream>
#include <QApplication>

#include <iostream>

GLint locVerts;

#define CHECKERR err = glGetError(); if ( err != GL_NO_ERROR ) { if ( err == GL_INVALID_OPERATION ) std::cout << "Error: INVALID_OPERATION Line " << __LINE__ << std::endl; else if (err == GL_INVALID_VALUE) std::cout << "Error: INVALID_VALUE Line " << __LINE__ << std::endl; else std::cout << "Error: Line " << __LINE__ << std::endl; }

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
    
    if ( !m_glProgram.bindAttributeLocation("v_color",20) )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));
CHECKERR
    if ( !m_glProgram.bindAttributeLocation("v_position",21) )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));
CHECKERR
    

    // link program
CHECKERR
    if ( !m_glProgram.link() )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));
CHECKERR

    // initialize triangle
    m_triangle.init(m_glProgram, "v_position", "v_color", "model");
CHECKERR
    m_triangle2.init(m_glProgram, "v_position", "v_color", "model");
CHECKERR

    // rotate then translate
    m_triangle.updateModel(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.25f,0.25f,0.0f)),180.0f, glm::vec3(0.0f, 0.0f, -1.0f)));
CHECKERR
    m_triangle2.updateModel(glm::translate(glm::mat4(1.0f), glm::vec3(-0.25,-0.25,0.0f)));
CHECKERR
}

void GLContext::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_glProgram.use();

    m_triangle.draw();
    m_triangle2.draw();
    
    m_glProgram.resetUsed();
}

bool GLContext::good() const
{
    return m_good;
}

