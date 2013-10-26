#include "Puck.hpp"

#include <glm/ext.hpp>

Puck::Puck()
{}

Puck::~Puck()
{
    if ( m_constraint.use_count() == 1 )
    {
        m_physicsWorld.removeConstraint(m_constraint.get());
        m_physicsWorld.removeRigidBody(m_rigidBody.get());
    }

    m_constraint.reset();
    m_rigidBody.reset();
    m_motionState.reset();
    m_collisionShape.reset();
}

bool Puck::init(const PhysicsWorld& world, const glm::vec3& position, double radius, double density, double friction, double restitution, const std::string& filename, bool flipUvs)
{
    if ( !Model::init(filename, flipUvs) )
        return false;

    // all models get scaled to fit inside 2x2x2 box during init (radius == 1.0)
    m_radius = radius;
    m_scale *= radius;
    float fRadius = radius;
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(fRadius, fRadius, fRadius));

    m_centerScaleMatrix = m_modelMatrix;
    m_height = (m_maxVertex.y - m_minVertex.y) * m_scale;

    m_restitution = restitution;
    m_friction = friction;
    m_position = position;

    // compute mass of puck
    m_mass = density * m_radius * m_radius * m_height * M_PI;

    // initialize physics
    m_physicsWorld = world;
    this->initPhysics();
    
    // update the modelView matrix
    this->updateTransform();

    return true;
}

void Puck::initPhysics()
{
    // initialize collision shape as cylinder
    m_collisionShape = std::shared_ptr<btCollisionShape>(new btCylinderShape(btVector3(btScalar(m_radius*2.0), btScalar(m_height), btScalar(m_radius*2.0))));
    m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
    
    // build motion state
    m_transform.setIdentity();
    m_transform.setOrigin(btVector3(0,0,0));
    m_motionState = std::shared_ptr<btDefaultMotionState>(new btDefaultMotionState(m_transform));
    
    // finish construction of rigid body
    btRigidBody::btRigidBodyConstructionInfo constructionParams(m_mass, m_motionState.get(), m_collisionShape.get(), m_inertia);
    constructionParams.m_friction = m_friction;
    constructionParams.m_restitution = m_restitution;
    m_rigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(constructionParams));
   
    // add body to world
    m_physicsWorld.addRigidBody(m_rigidBody.get());
    
    // added constraints
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
    m_rigidBody->setLinearFactor(btVector3(1,1,0));
    
    m_constraint = std::shared_ptr<btGeneric6DofConstraint>(new btGeneric6DofConstraint(*m_rigidBody, btTransform::getIdentity(), true));
  
    m_constraint->setLinearLowerLimit(btVector3(1,1,1));
    m_constraint->setLinearUpperLimit(btVector3(0,0,0));

    m_constraint->setAngularLowerLimit(btVector3(1,1,1));
    m_constraint->setAngularUpperLimit(btVector3(0,0,0));

    // set position of puck
    m_transform = m_rigidBody->getCenterOfMassTransform();
    m_transform.setOrigin(btVector3(m_position.x,m_position.y,m_position.z));
    m_rigidBody->setCenterOfMassTransform(m_transform);

    // add constraint
    m_physicsWorld.addConstraint(m_constraint.get());
}

void Puck::updateTransform()
{
    m_transform = m_rigidBody->getCenterOfMassTransform();
    
    const btVector3 bAxis = m_transform.getRotation().getAxis();
    const btVector3 bTranslation = m_transform.getOrigin();
    GLfloat angle = static_cast<GLfloat>(m_transform.getRotation().getAngle());

    const glm::vec3 translation(
        static_cast<GLfloat>(bTranslation.x()),
        static_cast<GLfloat>(bTranslation.y()),
        static_cast<GLfloat>(bTranslation.z()));

    const glm::vec3 axis(
        static_cast<GLfloat>(bAxis.x()),
        static_cast<GLfloat>(bAxis.y()),
        static_cast<GLfloat>(bAxis.z()));

    // encorporate translation and rotation of puck
    m_modelMatrix =
        glm::translate(
            glm::rotate(
                m_centerScaleMatrix,
                angle,
                axis
            ),
            translation
        );
}

