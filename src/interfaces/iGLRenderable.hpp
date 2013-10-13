#ifndef IGLRENDERABLE_HPP
#define IGLRENDERABLE_HPP

#include <glm/glm.hpp>
#include "../glwrappers/GLUniform.hpp"

const unsigned int TEXTURE_DIFFUSE  = 0;
const unsigned int TEXTURE_BUMP     = 1;
const unsigned int TEXTURE_SPECULAR = 2;

enum DrawType {DRAW_MATERIAL, DRAW_TEXTURE};

class iGLRenderable
{
public:
    iGLRenderable() {};
    virtual ~iGLRenderable() {};
    
    // This will be called
    virtual const glm::mat4& getModelMatrix() const = 0;
    virtual void draw(DrawType type = DRAW_MATERIAL) = 0;
    virtual void setUniforms(const std::vector<GLUniform>& uniforms, DrawType type = DRAW_MATERIAL) = 0;
};

#endif // IGLRENDERABLE_HPP

