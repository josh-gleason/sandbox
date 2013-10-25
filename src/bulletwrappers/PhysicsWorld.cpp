#include "PhysicsWorld.hpp"
#include <bullet/btBulletDynamicsCommon.h>

PhysicsWorld::PhysicsWorld()
{

}

PhysicsWorld::~PhysicsWorld()
{
    // deallocate in reverse order (if not owned by anyone else)
    m_world.reset();
    m_solver.reset();
    m_broadphase.reset();
    m_dispatcher.reset();
    m_collisionConfiguration.reset();
}

void PhysicsWorld::init()
{
    // allocate all necessary for the world
    m_collisionConfiguration = std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
    m_dispatcher             = std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher());
    m_broadphase             = std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
    m_solver                 = std::shared_ptr<btSequentialConstraintSolver>(new btSequentialImpulseConstraintSolver());
    m_world = std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
            m_dispatcher,
            m_broadphase,
            m_solver,
            m_collisionConfiguration));
}

