#ifndef MAINAPP_HPP 
#define MAINAPP_HPP

// need to be included first
#include "../glwrappers/GLProgram.hpp"
#include "../glwrappers/GLBuffer.hpp"
#include "../glwrappers/GLUniform.hpp"
#include "../view/Camera.hpp"
#include "../interfaces/iGLRenderable.hpp"

#include <QGLWidget>
#include <QTimer>

class QKeyEvent;

class MainApp : public QGLWidget
{
    Q_OBJECT
public:
    MainApp(const char* modelPath, QWidget *parent = nullptr);

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
    GLProgram              m_glProgramTexture;
    GLBuffer               m_glVertexPosBuffer;
    GLBuffer               m_glVertexIdxBuffer;
   
    // TODO use uniform buffer objects to share uniforms between programs
    GLUniform              m_uMatrixMvp_M;
    GLUniform              m_uMatrixMv_M;
    GLUniform              m_uMatrixNormal_M;
    GLUniform              m_uDiffuse_M;
    GLUniform              m_uSpecular_M;
    GLUniform              m_uAmbient_M;
    GLUniform              m_uShininess_M;
    GLUniform              m_uLightPos_M;
    GLUniform              m_uLightDiffuse_M;
    GLUniform              m_uLightSpecular_M;
    GLUniform              m_uLightAmbient_M;
   
    // texture program uniforms
    GLUniform              m_uMatrixMvp_T;
    GLUniform              m_uMatrixMv_T;
    GLUniform              m_uMatrixNormal_T;
    GLUniform              m_uDiffuse_T;
    GLUniform              m_uSpecular_T;
    GLUniform              m_uAmbient_T;
    GLUniform              m_uShininess_T;
    GLUniform              m_uLightPos_T;
    GLUniform              m_uLightDiffuse_T;
    GLUniform              m_uLightSpecular_T;
    GLUniform              m_uLightAmbient_T;
    GLUniform              m_uTexBlend_T;

    glm::mat4              m_projectionMatrix;
    Camera                 m_camera;    // stores/manipulates view matrix

    RenderList             m_renderTargets;

    QTimer                 m_timer;
    unsigned char          m_keyFlags;
    QPoint                 m_cursorPosition;
    bool                   m_ignoreNextMovement;
    bool                   m_mouseEnable;

    std::string            m_modelPath;
};

#endif // MAINAPP_HPP 

