#ifndef MAINAPP_HPP 
#define MAINAPP_HPP

// need to be included first
#include "../glwrappers/GLProgram.hpp"
#include "../glwrappers/GLBuffer.hpp"
#include "../glwrappers/GLUniform.hpp"
#include "../objects/Camera.hpp"
#include "../objects/Lights.hpp"
#include "../interfaces/iGLRenderable.hpp"

#include <QGLWidget>
#include <QTimer>

class QKeyEvent;

class MainApp : public QGLWidget
{
    Q_OBJECT
public:
    MainApp(const char* modelPath, bool flipUvs = false, QWidget *parent = nullptr);

    // check if initializeGL succeeded
    bool good() const;
public slots:
    void timerTick();
protected:
    typedef std::vector<std::shared_ptr<iGLRenderable>> RenderList;

    // exits the program
    void reportError(const QString& error);

    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

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
    Camera                 m_camera;    // stores/manipulates view matrix

    Lights                 m_lights;

    RenderList             m_renderTargets;

    QTimer                 m_timer;
    unsigned char          m_keyFlags;
    QPoint                 m_cursorPosition;
    bool                   m_ignoreNextMovement;
    bool                   m_mouseEnable;

    std::string            m_modelPath;
    bool                   m_flipUvs;
};

#endif // MAINAPP_HPP 

