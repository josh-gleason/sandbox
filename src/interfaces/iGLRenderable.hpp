#ifndef IGLRENDERABLE_HPP
#define IGLRENDERABLE_HPP

#include <glm/glm.hpp>
#include "../glwrappers/GLBuffer.hpp"

const unsigned int TEXTURE_DIFFUSE  = 0x1;
const unsigned int TEXTURE_SPECULAR = 0x2;
const unsigned int TEXTURE_BUMP     = 0x4;

// attribute locations
const GLuint V_POSITION = 0;
const GLuint V_NORMAL   = 1;
const GLuint V_UVCOORD  = 2;
const GLuint V_TANGENT  = 3;
const GLuint V_BINORMAL = 4;

const unsigned int TEXTURE_TYPES[] = {TEXTURE_DIFFUSE, TEXTURE_SPECULAR, TEXTURE_BUMP};

// TEXUTRE TYPES
//    D : Diffuse texture   (0x1)
//    S : Specular map      (0x2)
//    B : Bump map          (0x4)
// MATERIAL
//    Just material properties (no textures)
enum DrawType {
    DRAW_MATERIAL       = 0x0,
    DRAW_TEXTURE_D      = TEXTURE_DIFFUSE,
    DRAW_TEXTURE_S      = TEXTURE_SPECULAR,
    DRAW_TEXTURE_DS     = TEXTURE_DIFFUSE | TEXTURE_SPECULAR,
    DRAW_TEXTURE_B      = TEXTURE_BUMP,
    DRAW_TEXTURE_DB     = TEXTURE_DIFFUSE | TEXTURE_BUMP,
    DRAW_TEXTURE_SB     = TEXTURE_SPECULAR | TEXTURE_BUMP,
    DRAW_TEXTURE_DSB    = TEXTURE_DIFFUSE | TEXTURE_SPECULAR | TEXTURE_BUMP
};

enum UniformType {
    MATERIALS           = 0,
    TEXBLEND            = 1
};

// uniform block binding points
const GLuint UB_MATRICES = 1;
const GLuint UB_LIGHT    = 2;
const GLuint UB_MATERIAL = 3;

// uniform block offsets based on layout(std140)
const GLintptr MAT_MVP_OFFSET     = 0;
const GLintptr MAT_MV_OFFSET      = sizeof(glm::mat4);
const GLintptr MAT_NORMAL_OFFSET  = sizeof(glm::mat4)*2;

const GLintptr LIGHT_POSITION_OFFSET = 0;
const GLintptr LIGHT_DIFFUSE_OFFSET  = sizeof(glm::vec4);
const GLintptr LIGHT_SPECULAR_OFFSET = sizeof(glm::vec4)*2;
const GLintptr LIGHT_AMBIENT_OFFSET  = sizeof(glm::vec4)*3;

const GLintptr MATERIAL_DIFFUSE_OFFSET   = 0;
const GLintptr MATERIAL_SPECULAR_OFFSET  = sizeof(glm::vec4);
const GLintptr MATERIAL_AMBIENT_OFFSET   = sizeof(glm::vec4)*2;
const GLintptr MATERIAL_SHININESS_OFFSET = sizeof(GLfloat)*11;
const GLintptr MATERIAL_TEXBLEND_OFFSET  = sizeof(GLfloat)*12;

class iGLRenderable
{
public:
    iGLRenderable() {};
    virtual ~iGLRenderable() {};
    
    // This will be called
    virtual const glm::mat4& getModelMatrix() const = 0;
    virtual void draw(DrawType type = DRAW_MATERIAL) = 0;
    virtual void setUniforms(GLBuffer &ubo, UniformType type = MATERIALS) = 0;
};

#endif // IGLRENDERABLE_HPP

