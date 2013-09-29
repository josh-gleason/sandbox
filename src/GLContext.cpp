#include <GL/glew.h>

#include "GLContext.hpp"

#include "GLShader.hpp"
#include "GLUniform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <QTextStream>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>

#include <iostream>

const GLuint V_POSITION = 0;
const GLuint V_COLOR = 1;

const unsigned char MOVE_FORWARD  = 0x01;
const unsigned char MOVE_BACKWARD = 0x02;
const unsigned char MOVE_LEFT     = 0x04;
const unsigned char MOVE_RIGHT    = 0x08;
const unsigned char MOVE_UP       = 0x10;
const unsigned char MOVE_DOWN     = 0x20;
const unsigned char ROTATE_CCW    = 0x40;
const unsigned char ROTATE_CW     = 0x80;

#define CHECKERR err = glGetError(); if ( err != GL_NO_ERROR ) { if ( err == GL_INVALID_OPERATION ) std::cout << "Error: INVALID_OPERATION Line " << __LINE__ << std::endl; else if (err == GL_INVALID_VALUE) std::cout << "Error: INVALID_VALUE Line " << __LINE__ << std::endl; else std::cout << "Error: Line " << __LINE__ << std::endl; }

const float FPS = 60.0f;
const unsigned int TICK_RATE = 1000 * (1.0/FPS);

const float FOV_DEG = 45.0f;
const float FIELD_NEAR = 0.01f;
const float FIELD_FAR = 100.0f;

GLContext::GLContext(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent),
    m_good(true),
    //m_camera( glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode::CAMERA_Y_LOCK_VERT),
    m_camera( glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode::CAMERA_FREE),
    m_keyFlags(0)
{
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    m_timer.start(TICK_RATE);
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
    
    if ( !m_glProgram.bindAttributeLocation("v_color",V_COLOR) )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));
    if ( !m_glProgram.bindAttributeLocation("v_position",V_POSITION) )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));
    
    // link program
    if ( !m_glProgram.link() )
        return reportError(QString::fromUtf8(m_glProgram.getLastError().c_str()));

    m_glProgram.use();

    // get view and projection matrices (model handled by each shape)
    m_view.init(m_glProgram, "view", MAT4F);
    m_view.loadData(m_camera.getViewMatrix());
    m_view.set();

    // set perspective
    m_projection.init(m_glProgram, "projection", MAT4F);
    m_projection.loadData(glm::perspective(FOV_DEG, float(this->height())/float(this->width()), FIELD_NEAR, FIELD_FAR));
    m_projection.set();

    // initialize triangles
    m_triangle.init(m_glProgram, "v_position", "v_color", "model");
    m_triangle2.init(m_glProgram, "v_position", "v_color", "model");

    // rotate then translate
    m_triangle.updateModel(glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.25f,0.25f,0.0f)), 180.0f, glm::vec3(0.0f, 0.0f, -1.0f)),glm::vec3(0.33f,0.33f,1.0f)));
    m_triangle2.updateModel(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.25,-0.25,0.0f)), glm::vec3(0.25f,0.25f,1.0f)));
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

void GLContext::keyPressEvent(QKeyEvent *event)
{
    if ( event->isAutoRepeat() ) return;

    if ( event->key() == Qt::Key_W )
        m_keyFlags |= MOVE_FORWARD;
    else if ( event->key() == Qt::Key_S )
        m_keyFlags |= MOVE_BACKWARD;        
    else if ( event->key() == Qt::Key_A )
        m_keyFlags |= MOVE_LEFT;        
    else if ( event->key() == Qt::Key_D )
        m_keyFlags |= MOVE_RIGHT;
    else if ( event->key() == Qt::Key_Space )
        m_keyFlags |= MOVE_UP;
    else if ( event->key() == Qt::Key_Control )
        m_keyFlags |= MOVE_DOWN;
    else if ( event->key() == Qt::Key_Q )
        m_keyFlags |= ROTATE_CCW;
    else if ( event->key() == Qt::Key_E )
        m_keyFlags |= ROTATE_CW;
}

void GLContext::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->isAutoRepeat() ) return;

    if ( event->key() == Qt::Key_W )
        m_keyFlags &= (MOVE_FORWARD ^ 0xff);        
    else if ( event->key() == Qt::Key_S )
        m_keyFlags &= (MOVE_BACKWARD ^ 0xff);       
    else if ( event->key() == Qt::Key_A )
        m_keyFlags &= (MOVE_LEFT ^ 0xff);        
    else if ( event->key() == Qt::Key_D )
        m_keyFlags &= (MOVE_RIGHT ^ 0xff);
    else if ( event->key() == Qt::Key_Space )
        m_keyFlags &= (MOVE_UP ^ 0xff);
    else if ( event->key() == Qt::Key_Control )
        m_keyFlags &= (MOVE_DOWN ^ 0xff);
    else if ( event->key() == Qt::Key_Q )
        m_keyFlags &= (ROTATE_CCW ^ 0xff);
    else if ( event->key() == Qt::Key_E )
        m_keyFlags &= (ROTATE_CW ^ 0xff);
}

void GLContext::mouseMoveEvent(QMouseEvent * event)
{
    if ( !(event->buttons() & Qt::LeftButton) )
        return;

    GLfloat thetaVert = 2 * (m_previousMousePos.y() - event->y()) * (FOV_DEG / this->height());
    GLfloat thetaHoriz = 2 * (m_previousMousePos.x() - event->x()) * (FOV_DEG / this->width());

    m_camera.rotateVert(thetaVert);
    m_camera.rotateHoriz(thetaHoriz);

    m_previousMousePos = event->pos();

    // set the matrix
    m_view.loadData(m_camera.getViewMatrix());
    m_view.set();
    
    this->repaint();
}

void GLContext::mousePressEvent(QMouseEvent * event)
{
    if ( event->buttons() & Qt::LeftButton )
        m_previousMousePos = event->pos();
}

void GLContext::timerTick()
{
    if ( (m_keyFlags & MOVE_FORWARD) )
        m_camera.moveStraight(0.02f);
    if ( (m_keyFlags & MOVE_BACKWARD) )
        m_camera.moveStraight(-0.02f);
    if ( (m_keyFlags & MOVE_LEFT) )
        m_camera.moveHoriz(-0.02f);
    if ( (m_keyFlags & MOVE_RIGHT) )
        m_camera.moveHoriz(0.02f);
    if ( (m_keyFlags & MOVE_UP) )
        m_camera.moveVert(0.02f);
    if ( (m_keyFlags & MOVE_DOWN) )
        m_camera.moveVert(-0.02f);
    if ( (m_keyFlags & ROTATE_CCW) )
        m_camera.rotateStraight(1.f);
    if ( (m_keyFlags & ROTATE_CW) )
        m_camera.rotateStraight(-1.f);

    if ( m_keyFlags != 0 )
    {
        m_view.loadData(m_camera.getViewMatrix());
        m_view.set();
    }
    else    // do not repaint
    {
        return;
    }

    this->repaint();
}

bool GLContext::good() const
{
    return m_good;
}

