#ifndef IGLRENDERABLE_HPP
#define IGLRENDERABLE_HPP

class iGLRenderable
{
public:
    iGLRenderable() {};
    virtual ~iGLRenderable() {};
    
    // This will be called
    virtual void draw() = 0;
};

#endif // IGLRENDERABLE_HPP

