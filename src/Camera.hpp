#ifndef CAMERA_HPP
#define CAMERA_HPP

#define GLM_SWIZZLE
#include <GL/glew.h>
#include <glm/glm.hpp>

enum CameraMode {CAMERA_FREE, CAMERA_Y_LOCK_VERT, CAMERA_Y_LOCK_BOTH};

// The world has no well defined up in free mode

// CAMERA_FREE        full pitch/yaw/roll camera
// CAMERA_Y_LOCK_VERT horizontal movements are restricted to XZ plane and roll is
//                    disabled, vertical movement is restricted to Y-axis
// CAMERA_Y_LOCK_BOTH forward and horizontal movements are restricted to XZ plane
//                    and roll is disabled, vertical movement is restricted to
//                    Y-axis

// Note: Y_LOCK modes give a sense that the worlds Y axis is up

class Camera
{
public:
   Camera(glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), CameraMode mode = CAMERA_FREE);
   Camera(const Camera& rhs);

   Camera& operator=(const Camera& rhs);

   void resetView();
 
   // theta is in degrees
   void rotateStraight(GLfloat theta);
   void rotateVert(GLfloat theta);
   void rotateHoriz(GLfloat theta);

   void moveStraight(GLfloat dist);
   void moveHoriz(GLfloat dist);
   void moveVert(GLfloat dist);

   void setPosition(glm::vec4 position);
   glm::mat4 getTranslation() const;
   glm::mat4 getRotation() const;

   const glm::mat4& getViewMatrix();
protected:
   void updateView();

   glm::vec4 m_position;

   // direction camera is looking
   glm::vec4 m_orientation;

   // the up vector (rotate around this when looking left/right)
   glm::vec4 m_up;

   // the vector looking right (rotate around this when looking up/down)
   // (happens to be normal to the up and orientation vectors)
   glm::vec4 m_normal;

   // rotation and translation matrices for the camera
   glm::mat4 m_rotation;
   glm::mat4 m_translation;
   
   // the current view matrix for the camera
   glm::mat4 m_view;
   
   // if the same operation is done twice, the rotation matrix does
   // not need to be recomputed.  This is a special case but it should
   // come up quite often
   enum Dir {VERT, HORIZ, STRAIGHT, NONE};
   GLfloat m_previousTheta;
   Dir m_previousDirection;
   glm::mat4 m_previousRotation;

   CameraMode m_mode;
};

#endif // CAMERA_H

