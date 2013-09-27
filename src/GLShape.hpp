#ifndef GLSHAPE_HPP 
#define GLSHAPE_HPP

#include "iGLRenderable.hpp"

class GLShape : public iGLRenderable 
{
public:
    GLShape() {};
    virtual ~GLShape() {};
    
    virtual void draw();
};

#endif // GLSHAPE_HPP

