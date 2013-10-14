#ifndef IGLRENDERABLE_HPP
#define IGLRENDERABLE_HPP

#include <glm/glm.hpp>
#include "../glwrappers/GLBuffer.hpp"

const unsigned int TEXTURE_DIFFUSE  = 0x1;
const unsigned int TEXTURE_SPECULAR = 0x2;
const unsigned int TEXTURE_BUMP     = 0x4;

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
    MATERIALS           = 0
};

const GLintptr MATERIAL_DIFFUSE_OFFSET   = 0;
const GLintptr MATERIAL_SPECULAR_OFFSET  = sizeof(glm::vec4);
const GLintptr MATERIAL_AMBIENT_OFFSET   = sizeof(glm::vec4)*2;
const GLintptr MATERIAL_SHININESS_OFFSET = sizeof(GLfloat)*11;

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

