#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

// TODO : Replace the incremental changes to the matrices to use pre-stored angles for each degree
//        freedom.  This is to prevent the possibility of warping the view because of floating
//        point errors.

Camera::Camera(glm::vec4 position, CameraMode mode) :
    m_orientation(glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)),
    m_up(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
    m_normal(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
    m_previousDirection(NONE),
    m_mode(mode)
{
    m_translation = glm::mat4(1.0f);
    m_translation[3][0] = -position.x;
    m_translation[3][1] = -position.y;
    m_translation[3][2] = -position.z;

    this->updateView();
}

Camera& Camera::operator=(const Camera& rhs)
{
   m_orientation       = rhs.m_orientation;
   m_up                = rhs.m_up;
   m_normal            = rhs.m_normal;
   m_rotation          = rhs.m_rotation;
   m_translation       = rhs.m_translation;
   m_view              = rhs.m_view;
   m_previousTheta     = rhs.m_previousTheta;
   m_previousDirection = rhs.m_previousDirection;
   m_previousRotation  = rhs.m_previousRotation;
   m_mode              = rhs.m_mode;

   return *this;
}

void Camera::resetView()
{
    m_orientation = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    m_up = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    m_normal = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    updateView();
}

void Camera::rotateStraight(GLfloat theta)
{
    if ( m_mode == CAMERA_Y_LOCK_VERT || m_mode == CAMERA_Y_LOCK_BOTH )
        return;

    if ( m_previousDirection != STRAIGHT || m_previousTheta != theta )
    {
        m_previousRotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(m_orientation.xyz()));
        m_previousDirection = STRAIGHT;
        m_previousTheta = theta;
    }

    m_up = m_previousRotation * m_up;
    m_normal = m_previousRotation * m_normal;
    m_rotation = glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(m_orientation.xyz()), glm::vec3(m_up.xyz()));

    this->updateView();
}

void Camera::rotateVert(GLfloat theta)
{
    if ( m_previousDirection != VERT || m_previousTheta != theta )
    {
        m_previousRotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(m_normal.xyz()));
        m_previousDirection = VERT;
        m_previousTheta = theta;
    }
    
    m_orientation = m_previousRotation * m_orientation;
    m_up = m_previousRotation * m_up;

    m_rotation = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(m_orientation), glm::vec3(m_up));

    this->updateView();     
}

void Camera::rotateHoriz(GLfloat theta)
{
    if ( m_previousDirection != HORIZ || m_previousTheta != theta )
    {
        if ( m_mode == CAMERA_FREE )
            m_previousRotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(m_up.xyz()));
        else
            m_previousRotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f));

        m_previousDirection = HORIZ;
        m_previousTheta = theta;
    }

    m_orientation = m_previousRotation * m_orientation;
    m_normal = m_previousRotation * m_normal;

    // need to transform up vector during Y-lock mode
    if ( m_mode == CAMERA_Y_LOCK_VERT || m_mode == CAMERA_Y_LOCK_BOTH )
        m_up = m_previousRotation * m_up;

    m_rotation = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(m_orientation.xyz()), glm::vec3(m_up.xyz()));

    this->updateView();
}

void Camera::moveStraight(GLfloat dist)
{
    if ( m_mode == CAMERA_FREE || m_mode == CAMERA_Y_LOCK_VERT )
    {
        glm::vec4 translate = m_orientation * dist;

        m_translation[3][0] -= translate.x;
        m_translation[3][1] -= translate.y;
        m_translation[3][2] -= translate.z;
    }
    else // CAMERA_Y_LOCK_BOTH
    {
        glm::vec3 translate = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(m_normal.xyz()))*dist;

        m_translation[3][0] -= translate.x;
        m_translation[3][1] -= translate.y;
        m_translation[3][2] -= translate.z;
    }
    
    this->updateView();
}
void Camera::moveHoriz(GLfloat dist)
{
    glm::vec4 translate = m_normal*dist;

    m_translation[3][0] -= translate.x;
    m_translation[3][1] -= translate.y;
    m_translation[3][2] -= translate.z;

    this->updateView();
}

void Camera::moveVert(GLfloat dist)
{
    if ( m_mode == CAMERA_FREE )
    {
        glm::vec4 translate = m_up*dist;

        m_translation[3][0] -= translate.x;
        m_translation[3][1] -= translate.y;
        m_translation[3][2] -= translate.z;
    }
    else
    {
        // move up the y axis
        m_translation[3][1] -= dist;
    }

    this->updateView();
}

void Camera::setPosition(glm::vec4 position)
{
    m_translation[3][0] = -position.x;
    m_translation[3][1] = -position.y;
    m_translation[3][2] = -position.z;
    
    this->updateView();
}

glm::mat4 Camera::getTranslation() const
{
    return m_translation;
}

glm::mat4 Camera::getRotation() const
{
    return m_rotation;
}

const glm::mat4& Camera::getViewMatrix()
{
    return m_view;
}

void Camera::updateView()
{
    m_view = m_rotation * m_translation;
}

