#include <GL/glew.h>

#include "MainApp.hpp"

#include "../glwrappers/GLShader.hpp"
#include "../glwrappers/GLUniform.hpp"
#include "../shapes/Puck.hpp"
#include "../shapes/Table.hpp"

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
    
void printUniformOffsets(GLuint program, GLuint uniformBlock)
{
    GLchar name[256];
    GLint uniformCount;
    glGetActiveUniformBlockiv( program, uniformBlock, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount );

    GLint *indices = new GLint[uniformCount];
    glGetActiveUniformBlockiv( program, uniformBlock, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices );

    for ( GLint i = 0; i < uniformCount; ++i )
    {
        const GLuint index = (GLuint)indices[i];

        GLint type, offset;

        glGetActiveUniformName(program, index, 256, 0, name); 
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_TYPE, &type);
        glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_OFFSET, &offset);

        std::cout << " " << name << " (offset) : " << offset << std::endl;
    }
    std::cout << std::endl;

    delete [] indices;
}

MainApp::MainApp(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent),
    m_good(true),
    m_camera{Camera(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode::CAMERA_Y_LOCK_VERT),
             Camera(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode::CAMERA_Y_LOCK_VERT)},
    m_keyFlags(0),
    m_ignoreNextMovement(false),
    m_mouseEnable(false),
    m_cameraSelect(0)
{
    this->setCursor(QCursor(Qt::CrossCursor));
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    m_timer.start(TICK_RATE);
    m_time.start();
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
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);     // The Type Of Depth Test To Do
    glShadeModel(GL_SMOOTH);  // Enables Smooth Color Shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    CHECKERR
////////////////////////////////////////////// MATERIAL PROGRAM

    // initilize shaders
    GLShader vshaderMaterial;
    GLShader fshaderMaterial;
    GLShader vshaderTexD;
    GLShader fshaderTexD;
    if ( !vshaderMaterial.compileFromFile("./shaders/vshader.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderMaterial.getLastError().c_str()));
    if ( !fshaderMaterial.compileFromFile("./shaders/fshader.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderMaterial.getLastError().c_str()));
    if ( !vshaderTexD.compileFromFile("./shaders/vshaderTexD.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderTexD.getLastError().c_str()));
    if ( !fshaderTexD.compileFromFile("./shaders/fshaderTexD.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderTexD.getLastError().c_str()));
    
    // initialize material program
    m_glProgramMaterial.init(); 
    m_glProgramTexD.init();
   
    // attach shaders
    if ( !m_glProgramMaterial.attachShader(vshaderMaterial) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramMaterial.attachShader(fshaderMaterial) )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramTexD.attachShader(vshaderTexD) )
        return reportError(QString::fromUtf8(m_glProgramTexD.getLastError().c_str()));
    if ( !m_glProgramTexD.attachShader(fshaderTexD) )
        return reportError(QString::fromUtf8(m_glProgramTexD.getLastError().c_str()));

    // link program
    if ( !m_glProgramMaterial.link() )
        return reportError(QString::fromUtf8(m_glProgramMaterial.getLastError().c_str()));
    if ( !m_glProgramTexD.link() )
        return reportError(QString::fromUtf8(m_glProgramTexD.getLastError().c_str()));

    GLUniform diffuseMap;
    diffuseMap.init(m_glProgramTexD, "u_diffuseMap", INT);
    diffuseMap.loadData(0);
    diffuseMap.set();

    // get the uniform locations TODO make this in a class
    GLuint uMatrices = glGetUniformBlockIndex(m_glProgramMaterial.getProgramIdx(), "Matrices");
    if ( uMatrices == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Matrices" << std::endl;
    else
    {
        std::cout << "Matrices Block offsets :: " << std::endl;
        std::cout << " mvpMatrix : " << MAT_MVP_OFFSET << std::endl
                  << " mvMatrix  : " << MAT_MV_OFFSET << std::endl
                  << " normalMatrix : " << MAT_NORMAL_OFFSET << std::endl;
        printUniformOffsets(m_glProgramMaterial.getProgramIdx(), uMatrices);
    }

    GLuint uLights = glGetUniformBlockIndex(m_glProgramMaterial.getProgramIdx(), "Lights");
    if ( uLights == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Lights" << std::endl;
    else
    {
        std::cout << "Lights Block offsets :: " << std::endl;
        std::cout << " info.count      : " << LIGHT_COUNT_OFFSET << std::endl;
        for ( int i = 0; i < LIGHT_ARRAY_SIZE; ++i )
        {
            std::cout << " info.position[" << i << "]: " << LIGHT_ARRAY_OFFSET + i*LIGHT_ARRAY_STEP + LIGHT_ARRAY_POSITION_OFFSET << std::endl
                      << " info.diffuse[" << i << "] : " << LIGHT_ARRAY_OFFSET + i*LIGHT_ARRAY_STEP + LIGHT_ARRAY_DIFFUSE_OFFSET << std::endl
                      << " info.specular[" << i << "]: " << LIGHT_ARRAY_OFFSET + i*LIGHT_ARRAY_STEP + LIGHT_ARRAY_SPECULAR_OFFSET << std::endl
                      << " info.ambient[" << i << "] : " << LIGHT_ARRAY_OFFSET + i*LIGHT_ARRAY_STEP + LIGHT_ARRAY_AMBIENT_OFFSET << std::endl;
        }
        printUniformOffsets(m_glProgramMaterial.getProgramIdx(), uLights);
    }

    GLuint uMaterial = glGetUniformBlockIndex(m_glProgramMaterial.getProgramIdx(), "Material");
    if ( uMaterial == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Material" << std::endl;

    // bind uniform blocks to indices TODO make this in a class
    glUniformBlockBinding(m_glProgramMaterial.getProgramIdx(), uMatrices, UB_MATRICES);
    glUniformBlockBinding(m_glProgramMaterial.getProgramIdx(), uLights, UB_LIGHT);
    glUniformBlockBinding(m_glProgramMaterial.getProgramIdx(), uMaterial, UB_MATERIAL);
    
    // same for other program
    uMatrices = glGetUniformBlockIndex(m_glProgramTexD.getProgramIdx(), "Matrices");
    if ( uMatrices == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Matrices" << std::endl;
    uLights = glGetUniformBlockIndex(m_glProgramTexD.getProgramIdx(), "Lights");
    if ( uLights == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Lights" << std::endl;
    uMaterial = glGetUniformBlockIndex(m_glProgramTexD.getProgramIdx(), "Material");
    if ( uMaterial == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Material" << std::endl;
    else
    {
        std::cout << "Material Block offsets :: " << std::endl;
        std::cout << " diffuse  : " << MATERIAL_DIFFUSE_OFFSET << std::endl
                  << " specular : " << MATERIAL_SPECULAR_OFFSET << std::endl
                  << " ambient  : " << MATERIAL_AMBIENT_OFFSET << std::endl
                  << " shininess: " << MATERIAL_SHININESS_OFFSET << std::endl
                  << " texBlend : " << MATERIAL_TEXBLEND_OFFSET << std::endl;
        printUniformOffsets(m_glProgramTexD.getProgramIdx(), uMaterial);
    }
    
    glUniformBlockBinding(m_glProgramTexD.getProgramIdx(), uMatrices, UB_MATRICES);
    glUniformBlockBinding(m_glProgramTexD.getProgramIdx(), uLights, UB_LIGHT);
    glUniformBlockBinding(m_glProgramTexD.getProgramIdx(), uMaterial, UB_MATERIAL);

    m_glUniformMatrixBuffer.generate(1);
    m_glUniformLightsBuffer.generate(1);
    m_glUniformMaterialBuffer.generate(1);

    m_glUniformMatrixBuffer.bind(GL_UNIFORM_BUFFER);
    m_glUniformMatrixBuffer.setEmpty(MAT_BUFFER_SIZE, GL_DYNAMIC_DRAW);
    m_glUniformLightsBuffer.bind(GL_UNIFORM_BUFFER);
    m_glUniformLightsBuffer.setEmpty(LIGHT_BUFFER_SIZE, GL_DYNAMIC_DRAW);
    m_glUniformMaterialBuffer.bind(GL_UNIFORM_BUFFER);
    m_glUniformMaterialBuffer.setEmpty(MATERIAL_BUFFER_SIZE, GL_DYNAMIC_DRAW);

    m_glUniformMatrixBuffer.bindBase(UB_MATRICES);
    m_glUniformLightsBuffer.bindBase(UB_LIGHT);
    m_glUniformMaterialBuffer.bindBase(UB_MATERIAL);
   
    GLBuffer::unbindBuffers(GL_UNIFORM_BUFFER);

    // initialize physics
    m_physics.init();

    // initialize table
    std::shared_ptr<Table> table = std::shared_ptr<Table>(new Table);
    if ( !table->init(m_physics, glm::vec3(0.0f, 0.0f, 0.0f)) )
        return reportError("Unable to load table");
    table->setUniforms(m_glUniformMaterialBuffer, MATERIALS);
    m_renderTargets.push_back(std::shared_ptr<iGLRenderable>(table));
    m_physicsTargets.push_back(std::shared_ptr<iPhysicsObject>(table));

    // initialize puck
    std::shared_ptr<Puck> puck = std::shared_ptr<Puck>(new Puck);
    if ( !puck->init(m_physics, glm::vec3(0.0f, 0.0f, 0.0f), 0.1f) )
        return reportError("Unable to load puck");
    puck->setUniforms(m_glUniformMaterialBuffer, MATERIALS);
    m_renderTargets.push_back(std::shared_ptr<iGLRenderable>(puck));
    m_physicsTargets.push_back(std::shared_ptr<iPhysicsObject>(puck));

    // TODO temporary just to show physics works
    puck->setVelocity(glm::vec3(1.2f,0.0f,2.0f));

    // move the camera back and up
    m_camera[0].moveStraight(-3.0f);
    m_camera[0].moveHoriz(5.0f);
    m_camera[0].moveVert(2.0f);
    m_camera[0].rotateVert(-25.0f);
    m_camera[0].rotateHoriz(57.0f);
   
    m_camera[1].rotateHoriz(180.0);
    m_camera[1].moveStraight(-3.0f);
    m_camera[1].moveHoriz(5.0f);
    m_camera[1].moveVert(2.0f);
    m_camera[1].rotateVert(-25.0f);
    m_camera[1].rotateHoriz(57.0f);
    
    const LightInfo dark({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)});

    for ( int i = 0; i < LIGHT_ARRAY_SIZE; ++i )
        m_lights.addLight(dark);
   
    // turn light #0 on player 1s position
    const glm::vec3 position0 = m_camera[0].getTranslation()[3].xyz();
    const LightInfo lightInfo0({
        position0 * -1.0f,
        glm::vec3(1.0f,1.0f,1.0f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.0f, 0.0f, 0.0f)});
    m_lights.setLightInfo(lightInfo0, 0);
    
    // turn light #1 on player 1s position
    const glm::vec3 position1 = m_camera[1].getTranslation()[3].xyz();
    const LightInfo lightInfo1({
        position1 * -1.0f,
        glm::vec3(1.0f,1.0f,1.0f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.0f, 0.0f, 0.0f)});
    m_lights.setLightInfo(lightInfo1, 1);

#ifdef GRAPHICS_DEBUG
    std::cout << "Loading Wireframe Debug Program" << std::endl;

    GLShader vshaderWireframe;
    GLShader gshaderWireframe;
    GLShader fshaderWireframe;
    GLShader vshaderTexDWireframe;
    GLShader gshaderTexDWireframe;
    GLShader fshaderTexDWireframe;
    
    m_glProgramWireframe.init();
    m_glProgramTexDWireframe.init();

    // compile shaders
    if ( !vshaderWireframe.compileFromFile("./shaders/debug/vshaderWireframe.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderWireframe.getLastError().c_str()));
    if ( !gshaderWireframe.compileFromFile("./shaders/debug/gshaderWireframe.glsl", GL_GEOMETRY_SHADER) )
        return reportError(QString::fromUtf8(gshaderWireframe.getLastError().c_str()));
    if ( !fshaderWireframe.compileFromFile("./shaders/debug/fshaderWireframe.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderWireframe.getLastError().c_str()));
    if ( !vshaderTexDWireframe.compileFromFile("./shaders/debug/vshaderTexDWireframe.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderTexDWireframe.getLastError().c_str()));
    if ( !gshaderTexDWireframe.compileFromFile("./shaders/debug/gshaderTexDWireframe.glsl", GL_GEOMETRY_SHADER) )
        return reportError(QString::fromUtf8(gshaderTexDWireframe.getLastError().c_str()));
    if ( !fshaderTexDWireframe.compileFromFile("./shaders/debug/fshaderTexDWireframe.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderTexDWireframe.getLastError().c_str()));

    // attach shaders
    if ( !m_glProgramWireframe.attachShader(vshaderWireframe) )
        return reportError(QString::fromUtf8(m_glProgramWireframe.getLastError().c_str()));
    if ( !m_glProgramWireframe.attachShader(gshaderWireframe) )
        return reportError(QString::fromUtf8(m_glProgramWireframe.getLastError().c_str()));
    if ( !m_glProgramWireframe.attachShader(fshaderWireframe) )
        return reportError(QString::fromUtf8(m_glProgramWireframe.getLastError().c_str()));
    if ( !m_glProgramTexDWireframe.attachShader(vshaderTexDWireframe) )
        return reportError(QString::fromUtf8(m_glProgramTexDWireframe.getLastError().c_str()));
    if ( !m_glProgramTexDWireframe.attachShader(gshaderTexDWireframe) )
        return reportError(QString::fromUtf8(m_glProgramTexDWireframe.getLastError().c_str()));
    if ( !m_glProgramTexDWireframe.attachShader(fshaderTexDWireframe) )
        return reportError(QString::fromUtf8(m_glProgramTexDWireframe.getLastError().c_str()));
    
    // link programs
    if ( !m_glProgramWireframe.link() )
        return reportError(QString::fromUtf8(m_glProgramWireframe.getLastError().c_str()));
    if ( !m_glProgramTexDWireframe.link() )
        return reportError(QString::fromUtf8(m_glProgramTexDWireframe.getLastError().c_str()));

    // get and bind uniform block locations
    uMatrices = glGetUniformBlockIndex(m_glProgramWireframe.getProgramIdx(), "Matrices");
    if ( uMatrices == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Matrices" << std::endl;
    uLights = glGetUniformBlockIndex(m_glProgramWireframe.getProgramIdx(), "Lights");
    if ( uLights == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Lights" << std::endl;
    uMaterial = glGetUniformBlockIndex(m_glProgramWireframe.getProgramIdx(), "Material");
    if ( uMaterial == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Material" << std::endl;
    
    glUniformBlockBinding(m_glProgramWireframe.getProgramIdx(), uMatrices, UB_MATRICES);
    glUniformBlockBinding(m_glProgramWireframe.getProgramIdx(), uLights, UB_LIGHT);
    glUniformBlockBinding(m_glProgramWireframe.getProgramIdx(), uMaterial, UB_MATERIAL);

    uMatrices = glGetUniformBlockIndex(m_glProgramTexDWireframe.getProgramIdx(), "Matrices");
    if ( uMatrices == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Matrices" << std::endl;
    uLights = glGetUniformBlockIndex(m_glProgramTexDWireframe.getProgramIdx(), "Lights");
    if ( uLights == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Lights" << std::endl;
    uMaterial = glGetUniformBlockIndex(m_glProgramTexDWireframe.getProgramIdx(), "Material");
    if ( uMaterial == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Material" << std::endl;
    
    glUniformBlockBinding(m_glProgramTexDWireframe.getProgramIdx(), uMatrices, UB_MATRICES);
    glUniformBlockBinding(m_glProgramTexDWireframe.getProgramIdx(), uLights, UB_LIGHT);
    glUniformBlockBinding(m_glProgramTexDWireframe.getProgramIdx(), uMaterial, UB_MATERIAL);

    // set texture to sample from GL_TEXTURE0
    GLUniform diffuseMapWireframe;
    diffuseMapWireframe.init(m_glProgramTexDWireframe, "u_diffuseMap", INT);
    diffuseMapWireframe.loadData(0);
    diffuseMapWireframe.set();
#endif

#ifdef PHYSICS_DEBUG
    std::cout << "Loading Physics Debug OpenGL Program" << std::endl;
    
    GLShader vshaderDebug;
    GLShader fshaderDebug;
    if ( !vshaderDebug.compileFromFile("./shaders/debug/vshaderPassthrough.glsl", GL_VERTEX_SHADER) )
        return reportError(QString::fromUtf8(vshaderDebug.getLastError().c_str()));
    if ( !fshaderDebug.compileFromFile("./shaders/debug/fshaderPassthrough.glsl", GL_FRAGMENT_SHADER) )
        return reportError(QString::fromUtf8(fshaderDebug.getLastError().c_str()));

    m_glProgramDebug.init(); 

    if ( !m_glProgramDebug.attachShader(vshaderDebug) )
        return reportError(QString::fromUtf8(m_glProgramDebug.getLastError().c_str()));
    if ( !m_glProgramDebug.attachShader(fshaderDebug) )
        return reportError(QString::fromUtf8(m_glProgramDebug.getLastError().c_str()));
    if ( !m_glProgramDebug.link() )
        return reportError(QString::fromUtf8(m_glProgramDebug.getLastError().c_str()));
  
    uMatrices = glGetUniformBlockIndex(m_glProgramDebug.getProgramIdx(), "Matrices");
    if ( uMatrices == GL_INVALID_INDEX )
        std::cout << "Warning: Unable to find uniform block Matrices" << std::endl;
    
    glUniformBlockBinding(m_glProgramDebug.getProgramIdx(), uMatrices, UB_MATRICES);
  
    // tell physics world to use debug drawer
    m_physicsDebug = std::shared_ptr<PhysicsDebug>(new PhysicsDebug);
    m_physics.get()->setDebugDrawer(m_physicsDebug.get());
#endif
}

void MainApp::updatePhysicsObjects()
{
#ifdef PHYSICS_DEBUG 
    static int player = 0;

    // model matrix is always identity
    const glm::mat4 mvpMatrix = m_projectionMatrix * m_camera[player].getViewMatrix();

    // every other time this is called it toggles between players
    player = (player + 1)%2;

    m_glUniformMatrixBuffer.bind(GL_UNIFORM_BUFFER);
    m_glUniformMatrixBuffer.setSubData(&mvpMatrix, MAT_MVP_OFFSET); 

    m_glProgramDebug.use();
    
    m_physics.get()->debugDrawWorld();
    m_physicsDebug->loadToBuffer();
    m_physicsDebug->draw();

    GLProgram::resetUsed();
#endif

    for ( PhysicsList::iterator i = m_physicsTargets.begin(); i != m_physicsTargets.end(); ++i )
        (*i)->updateTransform();
}

void MainApp::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for ( int screenIdx = 0; screenIdx < 2; ++screenIdx )
    {
        // enable only half the screen
        glViewport(screenIdx * this->width()/2.0, 0.0, this->width()/2.0, this->height());
    
        // get the view matrix
        const glm::mat4 &viewMatrix = m_camera[screenIdx].getViewMatrix();
   
        // set lights
        m_lights.load(m_glUniformLightsBuffer, 0, viewMatrix);

        // update physics objects
        this->updatePhysicsObjects();

        // Render non-textured targets
#ifdef GRAPHICS_DEBUG
        m_glProgramWireframe.use();
#else
        m_glProgramMaterial.use();
#endif
        for ( RenderList::iterator i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i )
        {
            // set model uniforms
            const glm::mat4& modelMatrix = (*i)->getModelMatrix();
            const glm::mat4  modelViewMatrix = viewMatrix * modelMatrix;
            const glm::mat4  mvpMatrix = m_projectionMatrix * modelViewMatrix;
            const glm::mat4  normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));

            m_glUniformMatrixBuffer.bind(GL_UNIFORM_BUFFER);
            m_glUniformMatrixBuffer.setSubData(&mvpMatrix, MAT_MVP_OFFSET); 
            m_glUniformMatrixBuffer.setSubData(&modelViewMatrix, MAT_MV_OFFSET); 
            m_glUniformMatrixBuffer.setSubData(&normalMatrix, MAT_NORMAL_OFFSET); 

            (*i)->draw(DRAW_MATERIAL);
        }

#ifdef GRAPHICS_DEBUG
        m_glProgramTexDWireframe.use();
#else
        m_glProgramTexD.use();
#endif
        for ( RenderList::iterator i = m_renderTargets.begin(); i != m_renderTargets.end(); ++i )
        {
            // set model uniforms
            const glm::mat4& modelMatrix = (*i)->getModelMatrix();
            const glm::mat4  modelViewMatrix = viewMatrix * modelMatrix;
            const glm::mat4  mvpMatrix = m_projectionMatrix * modelViewMatrix;
            const glm::mat4  normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));

            m_glUniformMatrixBuffer.bind(GL_UNIFORM_BUFFER);
            m_glUniformMatrixBuffer.setSubData(&mvpMatrix, MAT_MVP_OFFSET); 
            m_glUniformMatrixBuffer.setSubData(&modelViewMatrix, MAT_MV_OFFSET); 
            m_glUniformMatrixBuffer.setSubData(&normalMatrix, MAT_NORMAL_OFFSET); 

            (*i)->draw(DRAW_TEXTURE_D);
        }
    }

    GLProgram::resetUsed();
}

void MainApp::keyPressEvent(QKeyEvent *event)
{
    if ( event->isAutoRepeat() ) return;
    
    const glm::vec3 position = m_camera[m_cameraSelect].getTranslation()[3].xyz();
    const LightInfo light({
        position * -1.0f,
        glm::vec3(1.0f,1.0f,1.0f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.0f, 0.0f, 0.0f)});
    
    static const LightInfo dark({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)});

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
    else if ( event->key() == Qt::Key_1 )
        m_lights.setLightInfo(light, 0);
    else if ( event->key() == Qt::Key_2 )
        m_lights.setLightInfo(light, 1);
    else if ( event->key() == Qt::Key_3 )
        m_lights.setLightInfo(light, 2);
    else if ( event->key() == Qt::Key_4 )
        m_lights.setLightInfo(light, 3);
    else if ( event->key() == Qt::Key_5 )
        m_lights.setLightInfo(light, 4);
    else if ( event->key() == Qt::Key_6 )
        m_lights.setLightInfo(light, 5);
    else if ( event->key() == Qt::Key_7 )
        m_lights.setLightInfo(light, 6);
    else if ( event->key() == Qt::Key_8 )
        m_lights.setLightInfo(light, 7);
    else if ( event->key() == Qt::Key_Exclam )
        m_lights.setLightInfo(dark, 0);
    else if ( event->key() == Qt::Key_At )
        m_lights.setLightInfo(dark, 1);
    else if ( event->key() == Qt::Key_NumberSign )
        m_lights.setLightInfo(dark, 2);
    else if ( event->key() == Qt::Key_Dollar )
        m_lights.setLightInfo(dark, 3);
    else if ( event->key() == Qt::Key_Percent )
        m_lights.setLightInfo(dark, 4);
    else if ( event->key() == Qt::Key_AsciiCircum )
        m_lights.setLightInfo(dark, 5);
    else if ( event->key() == Qt::Key_Ampersand )
        m_lights.setLightInfo(dark, 6);
    else if ( event->key() == Qt::Key_Asterisk )
        m_lights.setLightInfo(dark, 7);
    
    this->repaint();
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

    m_camera[m_cameraSelect].rotateVert(thetaVert);
    m_camera[m_cameraSelect].rotateHoriz(thetaHoriz);

    QCursor::setPos(m_cursorPosition);

    this->repaint();
}

void MainApp::resizeGL(int width, int height)
{
    m_projectionMatrix = glm::perspective(FOV_DEG, float(width/2.0)/float(height), FIELD_NEAR, FIELD_FAR); 
}

void MainApp::mousePressEvent(QMouseEvent * event)
{
    if ( (event->buttons() & Qt::LeftButton) != 0 )
    {
        // select camera where mouse was clicked
        if (event->posF().x() > this->width()/2.0)
            m_cameraSelect = 1;
        else
            m_cameraSelect = 0;

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
    // physics tick
    double dt = 1000.0 * m_time.elapsed();
    m_time.restart();
    m_physics.tick(dt); 
    
    // update camera
    if ( (m_keyFlags & MOVE_FORWARD) )
        m_camera[m_cameraSelect].moveStraight(0.02f);
    if ( (m_keyFlags & MOVE_BACKWARD) )
        m_camera[m_cameraSelect].moveStraight(-0.02f);
    if ( (m_keyFlags & MOVE_LEFT) )
        m_camera[m_cameraSelect].moveHoriz(-0.02f);
    if ( (m_keyFlags & MOVE_RIGHT) )
        m_camera[m_cameraSelect].moveHoriz(0.02f);
    if ( (m_keyFlags & MOVE_UP) )
        m_camera[m_cameraSelect].moveVert(0.02f);
    if ( (m_keyFlags & MOVE_DOWN) )
        m_camera[m_cameraSelect].moveVert(-0.02f);
    if ( (m_keyFlags & ROTATE_CCW) )
        m_camera[m_cameraSelect].rotateStraight(1.f);
    if ( (m_keyFlags & ROTATE_CW) )
        m_camera[m_cameraSelect].rotateStraight(-1.f);

    this->repaint();
}

bool MainApp::good() const
{
    return m_good;
}

