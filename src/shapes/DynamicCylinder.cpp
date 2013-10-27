#include "DynamicCylinder.hpp"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

DynamicCylinder::DynamicCylinder()
{}

DynamicCylinder::~DynamicCylinder()
{
    if ( m_rigidBody.use_count() == 1 )
        m_physicsWorld.removeRigidBody(m_rigidBody.get());
    m_rigidBody.reset();
    m_motionState.reset();
    m_collisionShape.reset();
}

void DynamicCylinder::updateTransform()
{
    m_transform = m_rigidBody->getCenterOfMassTransform();

#ifdef PHYSICS_DEBUG
    // draw shape in debug mode
    btCylinderShape* shape = reinterpret_cast<btCylinderShape*>(m_collisionShape.get());
    debug.drawCylinder(shape->getRadius(), shape->getHalfExtentsWithMargin().y(), shape->getUpAxis(), m_rigidBody->getCenterOfMassTransform(), btVector3(0.0,0.0,1.0));
    debug.loadToBuffer();
    debug.draw(DrawType::DRAW_MATERIAL);
#endif // PHYSICS_DEBUG
}

bool DynamicCylinder::initPhysics(const PhysicsWorld& world, const InitialParams& params)
{
    m_physicsWorld = world;
    m_cylinderParams = params;
  
    // compute mass from density
    m_mass = m_cylinderParams.density * m_cylinderParams.height * m_cylinderParams.radius * m_cylinderParams.radius * M_PI;

    // initialize collision shape as cylinder
    m_collisionShape = std::shared_ptr<btCollisionShape>(new btCylinderShape(
                btVector3(
                    m_cylinderParams.radius,
                    m_cylinderParams.height / 2.0,
                    m_cylinderParams.radius)));
    m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
    
    // build motion state
    m_transform.setIdentity();
    m_transform.setOrigin(btVector3(0,0,0));
    m_motionState = std::shared_ptr<btDefaultMotionState>(new btDefaultMotionState(m_transform));
    
    // finish construction of rigid body
    btRigidBody::btRigidBodyConstructionInfo constructionParams(m_mass, m_motionState.get(), m_collisionShape.get(), m_inertia);
    constructionParams.m_friction = m_cylinderParams.friction;
    constructionParams.m_restitution = m_cylinderParams.restitution;
    m_rigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(constructionParams));
   
    // add body to world
    m_physicsWorld.addRigidBody(m_rigidBody.get());

    // set position of cylinder
    m_transform = m_rigidBody->getCenterOfMassTransform();
    m_transform.setOrigin(m_cylinderParams.initialPosition);
    m_transform.setRotation(m_cylinderParams.initialRotation);
    m_rigidBody->setCenterOfMassTransform(m_transform);
    
    return true;
}

