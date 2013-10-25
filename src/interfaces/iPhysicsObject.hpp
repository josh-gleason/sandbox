#ifndef IPHYSICSOBJECT_HPP
#define IPHYSICSOBJECT_HPP

#include "../bulletwrappers/PhysicsWorld.hpp"

class iPhysicsObject
{
public:
    iPhysicsObject() {}
    virtual ~iPhysicsObject() {}

    virtual void initPhysics(const PhysicsWorld& world)
    {
        world = m_world;
    }
protected:
    PhysicsWorld m_world;
};

#endif // IPHYSICSOBJECT_HPP

