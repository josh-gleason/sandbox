#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <memory>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class PhysicsWorld
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void init();
protected:
    std::shared_ptr<btDefaultCollisionConfiguration>     m_configuration;
    std::shared_ptr<btCollisionDispatcher>               m_dispatcher;
    std::shared_ptr<btBroadphaseInterface>               m_broadphase;
    std::shared_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::shared_ptr<btDiscreteDynamicsWorld>             m_world;
};

#endif // PHYSICSWORLD_HPP

