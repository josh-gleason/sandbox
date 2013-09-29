#ifndef IGLRENDERABLE_HPP
#define IGLRENDERABLE_HPP

#include <glm/glm.hpp>

class iGLRenderable
{
public:
    iGLRenderable() {};
    virtual ~iGLRenderable() {};
    
    // This will be called
    virtual void draw(const glm::mat4 &viewProjection) = 0;
};

#endif // IGLRENDERABLE_HPP

