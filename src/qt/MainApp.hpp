#ifndef MAINAPP_HPP 
#define MAINAPP_HPP

// need to be included first
#include "../glwrappers/GLProgram.hpp"
#include "../glwrappers/GLBuffer.hpp"
#include "../glwrappers/GLUniform.hpp"
#include "../objects/Camera.hpp"
#include "../objects/Lights.hpp"
#include "../interfaces/iGLRenderable.hpp"
#include "../interfaces/iPhysicsObject.hpp"
#include "../bulletwrappers/PhysicsWorld.hpp"
#include "../shapes/Puck.hpp"

#ifdef PHYSICS_DEBUG
    #include "../debug/PhysicsDebug.hpp"
#endif

#include <QGLWidget>
#include <QTimer>
#include <QTime>

class QKeyEvent;

class MainApp : public QGLWidget
{
    Q_OBJECT
public:
    MainApp(QWidget *parent = nullptr);

    // check if initializeGL succeeded
    bool good() const;
public slots:
    void timerTick();
protected:
    typedef std::vector<std::shared_ptr<iGLRenderable>> RenderList;
    typedef std::vector<std::shared_ptr<iPhysicsObject>> PhysicsList;

    // exits the program
    void reportError(const QString& error);

    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void updatePhysicsObjects();

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void resizeGL(int width, int height);

    // set in initializeGL if failure occurs
    bool                   m_good;

    GLProgram              m_glProgramMaterial;
    GLProgram              m_glProgramTexD;
    GLBuffer               m_glUniformMatrixBuffer;
    GLBuffer               m_glUniformLightsBuffer;
    GLBuffer               m_glUniformMaterialBuffer;

    glm::mat4              m_projectionMatrix;
    Camera                 m_camera[2];    // stores/manipulates view matrix

    Lights                 m_lights;

    RenderList             m_renderTargets;
    PhysicsList            m_physicsTargets;

    QTimer                 m_timer;
    QTime                  m_time;
    unsigned char          m_keyFlags;
    QPoint                 m_cursorPosition;
    bool                   m_ignoreNextMovement;
    bool                   m_mouseEnable;
    
    PhysicsWorld           m_physics;
   
    int                    m_cameraSelect;

#ifdef GRAPHICS_DEBUG
    GLProgram m_glProgramTexDWireframe;
    GLProgram m_glProgramWireframe;
    
    GLUniform m_winSizeWireframe;
    GLUniform m_winSizeTexDWireframe;
#endif

#ifdef PHYSICS_DEBUG
    GLProgram m_glProgramDebug;
    std::shared_ptr<PhysicsDebug> m_physicsDebug; 
#endif
};

#endif // MAINAPP_HPP 

