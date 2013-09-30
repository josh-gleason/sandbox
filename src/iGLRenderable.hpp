#ifndef IGLRENDERABLE_HPP
#define IGLRENDERABLE_HPP

#include <glm/glm.hpp>

class iGLRenderable
{
public:
    iGLRenderable() {};
    virtual ~iGLRenderable() {};
    
    // This will be called
    virtual const glm::mat4& getModelMatrix() const = 0;
    virtual void draw() = 0;
};

#endif // IGLRENDERABLE_HPP

