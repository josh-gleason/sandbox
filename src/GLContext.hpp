#ifndef GLCONTEXT_HPP
#define GLCONTEXT_HPP

// need to be included first
#include "GLProgram.hpp"
#include "GLBuffer.hpp"
#include "GLUniform.hpp"
#include "Triangle.hpp"
#include "Camera.hpp"

#include <QGLWidget>
#include <QTimer>

class QKeyEvent;

class GLContext : public QGLWidget
{
    Q_OBJECT
public:
    GLContext(QWidget *parent = nullptr);

    // check if initializeGL succeeded
    bool good() const;
public slots:
    void timerTick();
protected:
    // exits the program
    void reportError(const QString& error);

    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);

    // set in initializeGL if failure occurs
    bool        m_good;

    GLProgram              m_glProgram;
    GLBuffer               m_glVertexPosBuffer;
    GLBuffer               m_glVertexIdxBuffer;
    GLUniform              m_view;
    GLUniform              m_projection;
    Camera                 m_camera;
    
    Triangle               m_triangle;
    Triangle               m_triangle2;

    QTimer                 m_timer;
    unsigned char          m_keyFlags;
    QPoint                 m_previousMousePos;
};

#endif // GLCONTEXT_HPP

