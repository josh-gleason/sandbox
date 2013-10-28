#ifndef IPHYSICSOBJECT_HPP
#define IPHYSICSOBJECT_HPP

// define PHYSICS_DEBUG during compile time with -DPHYSICS_DEBUG compiler flag

#ifdef PHYSICS_DEBUG 
    #define PHYSICS_DEBUG_DEFINE(a) a
    #include "../shapes/PhysicsDebug.hpp"
#else
    #define PHYSICS_DEBUG_DEFINE(a) /*a*/
#endif

class iPhysicsObject
{
public:
    iPhysicsObject() {}
    virtual ~iPhysicsObject() {}
    
    virtual void updateTransform() = 0;
protected:
};

#endif // IPHYSICSOBJECT_HPP

