#include <GL/glew.h>

#include "GLContext.hpp"

#include "GLShader.hpp"
#include "GLUniform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <QTextStream>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QCursor>
#include <QResizeEvent>

#include <iostream>

#include "Model.hpp"

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

GLContext::GLContext(const char* modelPath, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent),
    m_good(true),
    m_camera( glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode::CAMERA_Y_LOCK_VERT),
    m_keyFlags(0),
    m_ignoreNextMovement(false),
    m_mouseEnable(false),
    m_modelPath(modelPath)
{
    this->setCursor(QCursor(Qt::CrossCursor));
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
  
    glEnable(GL_DEPTH_TEST);  // Enables Depth Testing
    glEnable(GL_DOUBLEBUFFER);
    glDepthFunc(GL_LESS);     // The Type Of Depth Test To Do
    glShadeModel(GL_SMOOTH);  // Enables Smooth Color Shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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


    // get the uniform locations
    m_uniformMatrixMvp.init(m_glProgram, "u_mvpMatrix", MAT4F); 

    GLAttribute vPosition;
    vPosition.init(m_glProgram, "v_position");

    GLAttribute vNormal;
    vNormal.init(m_glProgram, "v_normal");

    GLAttribute vUvCoord;
    vUvCoord.init(m_glProgram, "v_uvCoord");
    glVertexAttrib2f(vUvCoord.getAttributeIdx(), -1.0f, -1.0f);

    GLUniform uColor;
    uColor.init(m_glProgram, "u_color", VEC3F);

    // initialize triangles
    Model* model = new Model;
    if ( !model->init(m_modelPath, vPosition, vNormal, vUvCoord, uColor) )
        return reportError("Unable to load model");

    // add model to render list
    m_renderTargets.push_back(std::shared_ptr<iGLRenderable>(model));

    // move the camera back and up
    m_camera.moveStraight(-0.3f);
    m_camera.moveVert(0.2f);
    m_camera.rotateVert(-15.0f);
}

void GLContext::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // compute the view-projection matrix (the model matrix is multiplied by this for each shape)
    glm::mat4 viewProjectionMatrix = m_projectionMatrix * m_camera.getViewMatrix();

    // Render all targets in list
    m_glProgram.use();
    for ( RenderList::iterator i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i )
    {
        m_uniformMatrixMvp.loadData(viewProjectionMatrix * (*i)->getModelMatrix());
        m_uniformMatrixMvp.set();
        (*i)->draw();
    }
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
    if ( !m_mouseEnable )
        return;

    GLfloat thetaVert = 2 * (m_cursorPosition.y() - event->globalY()) * (FOV_DEG / this->height());
    GLfloat thetaHoriz = 2 * (m_cursorPosition.x() - event->globalX()) * (FOV_DEG / this->width());

    m_camera.rotateVert(thetaVert);
    m_camera.rotateHoriz(thetaHoriz);

    QCursor::setPos(m_cursorPosition);

    this->repaint();
}

void GLContext::resizeGL(int width, int height)
{
    m_projectionMatrix = glm::perspective(FOV_DEG, float(width)/float(height), FIELD_NEAR, FIELD_FAR); 
    glViewport(0, 0, width, height);
}

void GLContext::mousePressEvent(QMouseEvent * event)
{
    if ( (event->buttons() & Qt::LeftButton) != 0 )
    {
        m_mouseEnable = true;
        m_cursorPosition = event->globalPos();
        this->setCursor(Qt::BlankCursor);
    }
}

void GLContext::mouseReleaseEvent(QMouseEvent * event)
{
    if ( (event->buttons() & Qt::LeftButton) == 0 && m_mouseEnable )
    {
        m_mouseEnable = false;
        this->setCursor(Qt::CrossCursor);
    }
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

    if ( m_keyFlags == 0 ) // do not repaint
        return;
    else
        this->repaint();
}

bool GLContext::good() const
{
    return m_good;
}

