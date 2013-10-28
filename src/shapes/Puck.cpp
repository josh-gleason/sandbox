#include "Puck.hpp"

#include <glm/ext.hpp>

Puck::Puck() : Model::Model(), DynamicCylinder::DynamicCylinder()
{}

Puck::~Puck()
{
    if ( m_constraint.use_count() == 1 )
        m_physicsWorld.removeConstraint(m_constraint.get());
    m_constraint.reset();
}

bool Puck::init(const PhysicsWorld& world, const glm::vec3& position, double radius, double density, double friction, double restitution, const std::string& filename, bool flipUvs)
{
    if ( !Model::init(filename, flipUvs) )
        return false;

    // all models get scaled to fit inside 2x2x2 box during init (radius == 1.0)
    m_scale *= radius;
    float fRadius = radius;
    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(fRadius, fRadius, fRadius)) * m_modelMatrix;
    
    // this is the matrix to which all rotations and translations are applied
    m_centerScaleMatrix = m_modelMatrix;

    // set parameters for dynamic cylinder
    InitialParams params;
    params.radius = radius;
    params.height = (m_maxVertex.y - m_minVertex.y) * m_scale;
    params.restitution = restitution;
    params.friction = friction;
    params.density = density;
    params.initialPosition = btVector3(
        static_cast<btScalar>(position.x),
        static_cast<btScalar>(position.y),
        static_cast<btScalar>(position.z));
    btQuaternion q;
    q.setRotation(btVector3(0.0,0.0,1.0),0.01);
    params.initialRotation = q; 

    // initialize physics
    if ( !initPhysics(world, params) )
        return false;
    
    // update the modelView matrix
    this->updateTransform();

    return true;
}

bool Puck::initPhysics(const PhysicsWorld& world, const InitialParams& params)
{
    if ( !DynamicCylinder::initPhysics(world, params) )
        return false;

    // create constraints
    //m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
    //m_rigidBody->setLinearFactor(btVector3(1,0,1));
    //m_rigidBody->setAngularFactor(btVector3(1,1,1));

#if 0 
    m_constraint = std::shared_ptr<btGeneric6DofConstraint>(new btGeneric6DofConstraint(*m_rigidBody, btTransform::getIdentity(), true));
  
    m_constraint->setLinearLowerLimit(btVector3(1,1,1));
    m_constraint->setLinearUpperLimit(btVector3(0,0,0));

    m_constraint->setAngularLowerLimit(btVector3(1,1,1));
    m_constraint->setAngularUpperLimit(btVector3(0,0,0));

    // add constraint to world
    m_physicsWorld.addConstraint(m_constraint.get());
    ///////////////////////////// TODO TEMPORARY /////////////////////////////////////
    // add simple ground
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
    btRigidBody::btRigidBodyConstructionInfo
            groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    m_physicsWorld.addRigidBody(groundRigidBody); 
    ///////////////////////////// TODO END TEMPORARY /////////////////////////////////
#endif 
    return true;
}

void Puck::updateTransform()
{
    DynamicCylinder::updateTransform();

    // transform model matrix using updated physics matrix
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
    m_modelMatrix = glm::translate(glm::mat4(1.0),translation) * glm::rotate(glm::mat4(1.0),static_cast<GLfloat>(180.0 * angle / M_PI),axis) * m_centerScaleMatrix;
}

