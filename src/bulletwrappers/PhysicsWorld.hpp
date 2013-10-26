#ifndef PHYSICSWORLD_HPP
#define PHYSICSWORLD_HPP

#include <memory>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btGeneric6DofConstraint;

class PhysicsWorld
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void init();
    
    void removeRigidBody(btRigidBody* body);
    void addRigidBody(btRigidBody* body);
    void addConstraint(btGeneric6DofConstraint* body);
    void removeConstraint(btGeneric6DofConstraint* constraint);
    void tick(double dt);
protected:
    std::shared_ptr<btDefaultCollisionConfiguration>     m_configuration;
    std::shared_ptr<btCollisionDispatcher>               m_dispatcher;
    std::shared_ptr<btBroadphaseInterface>               m_broadphase;
    std::shared_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::shared_ptr<btDiscreteDynamicsWorld>             m_world;
};

#endif // PHYSICSWORLD_HPP

