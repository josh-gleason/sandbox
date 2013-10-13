#include <GL/glew.h>

#include "MainApp.hpp"

#include "../glwrappers/GLShader.hpp"
#include "../glwrappers/GLUniform.hpp"
#include "../shapes/Model.hpp"

// Qt includes
#include <QTextStream>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QCursor>
#include <QResizeEvent>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

// c++ libraries
#include <iostream>

const GLuint V_POSITION = 0;
const GLuint V_NORMAL   = 1;
const GLuint V_UVCOORD  = 2;
const GLuint V_TANGENT  = 3;
const GLuint V_BINORMAL = 4;

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

MainApp::MainApp(const char* modelPath, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent),
    m_good(true),
    m_camera(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode::CAMERA_Y_LOCK_VERT),
    m_keyFlags(0),
    m_ignoreNextMovement(false),
    m_mouseEnable(false),
    m_modelPath(modelPath)
{
    this->setCursor(QCursor(Qt::CrossCursor));
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    m_timer.start(TICK_RATE);
}

void MainApp::reportError(const QString& error)
{
    QTextStream qerr(stderr);
    qerr << "Error: " << error << endl;
    m_good = false;

    // won't work in initializeGL because main loop has not been entered yet
    qApp->quit();
}

void MainApp::initializeGL()
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

////////////////////////////////////////////// MATERIAL PROGRAM

    // initilize shaders
    GLShader vshaderMaterial;
    GLShader fshaderMaterial;
    if ( !vshaderMaterial.compileFromFile("./shaders/vshader.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderMaterial.getLastError().c_str()));
    if ( !fshaderMaterial.compileFromFile("./shaders/fshader.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderMaterial.getLastError().c_str()));
    
    // initialize material program
    m_glProgramMaterial.init(); 
   
    // attach shaders
    if ( !m_glProgramMaterial.attachShader(vshaderMaterial) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramMaterial.attachShader(fshaderMaterial) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));

    // set attribute locations
    if ( !m_glProgramMaterial.bindAttributeLocation("v_normal",V_NORMAL) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramMaterial.bindAttributeLocation("v_position",V_POSITION) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    
    // link program
    if ( !m_glProgramMaterial.link() )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));

    // get the uniform locations
    m_uMatrixMvp_M.init(m_glProgramMaterial, "u_mvpMatrix", MAT4F); 
    m_uMatrixMv_M.init(m_glProgramMaterial, "u_mvMatrix", MAT4F); 
    m_uMatrixNormal_M.init(m_glProgramMaterial, "u_normalMatrix", MAT4F); 
    m_uLightPos_M.init(m_glProgramMaterial, "u_lightPos", VEC3F); 
    m_uLightDiffuse_M.init(m_glProgramMaterial, "u_lightDiffuse", VEC3F); 
    m_uLightSpecular_M.init(m_glProgramMaterial, "u_lightSpecular", VEC3F); 
    m_uLightAmbient_M.init(m_glProgramMaterial, "u_lightAmbient", VEC3F); 
    m_uSpecular_M.init(m_glProgramMaterial, "u_specular", VEC3F);
    m_uDiffuse_M.init(m_glProgramMaterial, "u_diffuse", VEC3F);
    m_uAmbient_M.init(m_glProgramMaterial, "u_ambient", VEC3F);
    m_uShininess_M.init(m_glProgramMaterial, "u_shininess", FLOAT);

////////////////////////////////////////////// TEXTURE PROGRAM

    GLShader vshaderTexture;
    GLShader fshaderTexture;
    if ( !vshaderTexture.compileFromFile("./shaders/vshaderTexDSB.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderTexture.getLastError().c_str()));
    if ( !fshaderTexture.compileFromFile("./shaders/fshaderTexDSB.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderTexture.getLastError().c_str()));
    
    // initialize material program
    m_glProgramTexture.init(); 
   
    // attach shaders
    if ( !m_glProgramTexture.attachShader(vshaderTexture) )
        return reportError(QString::fromUtf8(m_glProgramTexture.getLastError().c_str()));
    if ( !m_glProgramTexture.attachShader(fshaderTexture) )
        return reportError(QString::fromUtf8(m_glProgramTexture.getLastError().c_str()));

    // set attribute locations
    if ( !m_glProgramTexture.bindAttributeLocation("v_normal",V_NORMAL) )
        return reportError(QString::fromUtf8(m_glProgramTexture.getLastError().c_str()));
    if ( !m_glProgramTexture.bindAttributeLocation("v_position",V_POSITION) )
        return reportError(QString::fromUtf8(m_glProgramTexture.getLastError().c_str()));
    if ( !m_glProgramTexture.bindAttributeLocation("v_uvCoord",V_UVCOORD) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramTexture.bindAttributeLocation("v_tangent",V_TANGENT) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramTexture.bindAttributeLocation("v_binormal",V_BINORMAL) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    
    // link program
    if ( !m_glProgramTexture.link() )
        return reportError(QString::fromUtf8(m_glProgramTexture.getLastError().c_str()));

    // get the uniform locations
    m_uMatrixMvp_T.init(m_glProgramTexture, "u_mvpMatrix", MAT4F); 
    m_uMatrixMv_T.init(m_glProgramTexture, "u_mvMatrix", MAT4F); 
    m_uMatrixNormal_T.init(m_glProgramTexture, "u_normalMatrix", MAT4F); 
    m_uLightPos_T.init(m_glProgramTexture, "u_lightPos", VEC3F); 
    m_uLightDiffuse_T.init(m_glProgramTexture, "u_lightDiffuse", VEC3F); 
    m_uLightSpecular_T.init(m_glProgramTexture, "u_lightSpecular", VEC3F); 
    m_uLightAmbient_T.init(m_glProgramTexture, "u_lightAmbient", VEC3F); 
    m_uSpecular_T.init(m_glProgramTexture, "u_specular", VEC3F);
    m_uDiffuse_T.init(m_glProgramTexture, "u_diffuse", VEC3F);
    m_uAmbient_T.init(m_glProgramTexture, "u_ambient", VEC3F);
    m_uShininess_T.init(m_glProgramTexture, "u_shininess", FLOAT);
    m_uTexBlend_T.init(m_glProgramTexture, "u_texBlend", FLOAT);
   
    // set which textures correspond to which samplers
    GLUniform uDiffuseMap, uSpecularMap, uBumpMap;
    uDiffuseMap.init(m_glProgramTexture, "u_diffuseMap", INT);// VEC3F);
    uDiffuseMap.loadData(TEXTURE_DIFFUSE);
    uDiffuseMap.set();
    uSpecularMap.init(m_glProgramTexture, "u_specularMap", INT);// VEC3F);
    uSpecularMap.loadData(TEXTURE_SPECULAR);
    uSpecularMap.set();
    uBumpMap.init(m_glProgramTexture, "u_bumpMap", INT); //VEC3F);
    uBumpMap.loadData(TEXTURE_BUMP);
    uBumpMap.set();

///////////////////////////////////////////////

    // TODO this init is unneccessary fix structure
    GLAttribute vPosition, vNormal, vUvCoord, vTangent, vBinormal;
    vPosition.init(m_glProgramMaterial, "v_position");
    vNormal.init(m_glProgramMaterial, "v_normal");
    vUvCoord.init(m_glProgramTexture, "v_uvCoord");
    vTangent.init(m_glProgramTexture, "v_tangent");
    vBinormal.init(m_glProgramTexture, "v_binormal");

    // initialize triangles
    Model* model = new Model;
    if ( !model->init(m_modelPath, vPosition, vNormal, vTangent, vBinormal, vUvCoord) )
        return reportError("Unable to load model");

    // add model to render list
    m_renderTargets.push_back(std::shared_ptr<iGLRenderable>(model));

    // move the camera back and up
    m_camera.moveStraight(-3.0f);
    m_camera.moveVert(2.0f);
    m_camera.rotateVert(-15.0f);
}

void MainApp::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // compute the view-projection matrix (the model matrix is multiplied by this for each shape)
    const glm::mat4 &viewMatrix = m_camera.getViewMatrix();

    std::vector<GLUniform> materialUniforms(4);
    std::vector<GLUniform> textureUniforms(5);

    materialUniforms[0] = m_uDiffuse_M;
    materialUniforms[1] = m_uSpecular_M;
    materialUniforms[2] = m_uAmbient_M;
    materialUniforms[3] = m_uShininess_M;

    textureUniforms[0] = m_uTexBlend_T;
    textureUniforms[1] = m_uDiffuse_T;
    textureUniforms[2] = m_uSpecular_T;
    textureUniforms[3] = m_uAmbient_T;
    textureUniforms[4] = m_uShininess_T;

    // Render non-textured targets
    m_glProgramMaterial.use();
    for ( RenderList::iterator i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i )
    {
        // set model uniforms
        const glm::mat4 &modelMatrix = (*i)->getModelMatrix();
        const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        m_uMatrixMvp_M.loadData(m_projectionMatrix * modelViewMatrix);
        m_uMatrixMv_M.loadData(modelViewMatrix);
        m_uMatrixNormal_M.loadData(glm::transpose(glm::inverse(modelViewMatrix)));
        m_uLightPos_M.loadData((viewMatrix*glm::vec4(4.0f, 4.0f, 4.0f, 1.0f)).xyz());
        m_uLightSpecular_M.loadData(glm::vec3(0.5f, 0.7f, 0.3f));
        m_uLightDiffuse_M.loadData(glm::vec3(0.8f, 0.6f, 0.3f));
        m_uLightAmbient_M.loadData(glm::vec3(0.4f, 0.4f, 0.4f));
        m_uMatrixMvp_M.set();
        m_uMatrixMv_M.set();
        m_uMatrixNormal_M.set();
        m_uLightSpecular_M.set();
        m_uLightDiffuse_M.set();
        m_uLightAmbient_M.set();

        (*i)->setUniforms(materialUniforms, DRAW_MATERIAL);
        (*i)->draw(DRAW_MATERIAL);
    }
   
    // Render textured targets
    m_glProgramTexture.use();
    for ( RenderList::iterator i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i )
    {
        // set model uniforms
        const glm::mat4 &modelMatrix = (*i)->getModelMatrix();
        const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        m_uMatrixMvp_T.loadData(m_projectionMatrix * modelViewMatrix);
        m_uMatrixMv_T.loadData(modelViewMatrix);
        m_uMatrixNormal_T.loadData(glm::transpose(glm::inverse(modelViewMatrix)));
        m_uLightPos_T.loadData(glm::vec3(1.0f, 3.0f, 4.0f));
        m_uLightSpecular_T.loadData(glm::vec3(1.0f, 1.0f, 1.0f));
        m_uLightDiffuse_T.loadData(glm::vec3(1.0f, 1.0f, 1.0f));
        m_uLightAmbient_T.loadData(glm::vec3(0.2f, 0.2f, 0.2f));
        m_uMatrixMvp_T.set();
        m_uMatrixMv_T.set();
        m_uMatrixNormal_T.set();
        m_uLightSpecular_T.set();
        m_uLightDiffuse_T.set();
        m_uLightAmbient_T.set();

        (*i)->setUniforms(textureUniforms, DRAW_TEXTURE_DSB);
        (*i)->draw(DRAW_TEXTURE_DSB);
    }
    m_glProgramTexture.resetUsed();
}

void MainApp::keyPressEvent(QKeyEvent *event)
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
    else if ( event->key() == Qt::Key_Escape )
        qApp->quit();
}

void MainApp::keyReleaseEvent(QKeyEvent *event)
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

void MainApp::mouseMoveEvent(QMouseEvent * event)
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

void MainApp::resizeGL(int width, int height)
{
    m_projectionMatrix = glm::perspective(FOV_DEG, float(width)/float(height), FIELD_NEAR, FIELD_FAR); 
    glViewport(0, 0, width, height);
}

void MainApp::mousePressEvent(QMouseEvent * event)
{
    if ( (event->buttons() & Qt::LeftButton) != 0 )
    {
        m_mouseEnable = true;
        m_cursorPosition = event->globalPos();
        this->setCursor(Qt::BlankCursor);
    }
}

void MainApp::mouseReleaseEvent(QMouseEvent * event)
{
    if ( (event->buttons() & Qt::LeftButton) == 0 && m_mouseEnable )
    {
        m_mouseEnable = false;
        this->setCursor(Qt::CrossCursor);
    }
}

void MainApp::timerTick()
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

bool MainApp::good() const
{
    return m_good;
}

