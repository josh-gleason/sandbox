#include "StaticMesh.hpp"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

StaticMesh::StaticMesh()
{}

StaticMesh::~StaticMesh()
{
    this->reset();
}

void StaticMesh::reset()
{
    if ( m_rigidBody.use_count() == 1 )
        m_physicsWorld.removeRigidBody(m_rigidBody.get());
    m_rigidBody.reset();
    m_motionState.reset();
    m_mesh.reset();
}

void StaticMesh::updateTransform()
{
    // it's static, don't need to do anything
}

bool StaticMesh::initPhysics(const PhysicsWorld& world, const InitialParams& params)
{
    m_physicsWorld = world;
    m_meshParams = params;

    // load triangle mesh
    if ( !m_mesh.loadMesh(params.filename, params.scale) )
        return false;
    
    // define motion state
    btTransform transform;
    transform.setOrigin(params.position);
    transform.setRotation(params.rotation);
    m_motionState = std::shared_ptr<btDefaultMotionState>(new btDefaultMotionState(transform));

    // create rigid body
    btVector3 localInertia(0,0,0);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, m_motionState.get(), m_mesh.getShape(), localInertia);
    rbInfo.m_friction = params.friction;
    rbInfo.m_restitution = params.restitution;
    m_rigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(rbInfo));

    // add static mesh to world
    m_physicsWorld.addRigidBody(m_rigidBody.get());

    return true;
}

