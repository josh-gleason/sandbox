#include "PhysicsWorld.hpp"
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.h>

PhysicsWorld::PhysicsWorld()
{}

PhysicsWorld::~PhysicsWorld()
{
    // deallocate in reverse order (if not owned by anyone else)
    m_world.reset();
    m_solver.reset();
    m_broadphase.reset();
    m_dispatcher.reset();
    m_configuration.reset();
}

void PhysicsWorld::init()
{
    // allocate all necessary for the world
    m_configuration = std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
    m_dispatcher             = std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher(m_configuration.get()));
    m_broadphase             = std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
    m_solver                 = std::shared_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());

    // create physics world
    m_world = std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
            m_dispatcher.get(),
            m_broadphase.get(),
            m_solver.get(),
            m_configuration.get()));
}

void PhysicsWorld::tick(double dt)
{
    m_world->stepSimulation(static_cast<btScalar>(dt));
}

void PhysicsWorld::removeRigidBody(btRigidBody* body)
{
    m_world->removeRigidBody(body);
}

void PhysicsWorld::addRigidBody(btRigidBody* body)
{
    m_world->addRigidBody(body);
}

void PhysicsWorld::addConstraint(btGeneric6DofConstraint* constraint)
{
    m_world->addConstraint(constraint);
}

void PhysicsWorld::removeConstraint(btGeneric6DofConstraint* constraint)
{
    m_world->removeConstraint(constraint);
}

