#ifndef IPHYSICSOBJECT_HPP
#define IPHYSICSOBJECT_HPP

class iPhysicsObject
{
public:
    iPhysicsObject() {}
    virtual ~iPhysicsObject() {}
    
    virtual void updateTransform() = 0;
protected:
};

#endif // IPHYSICSOBJECT_HPP

