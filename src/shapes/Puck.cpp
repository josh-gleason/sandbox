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

    // set parameters for dynamic cylinder
    InitialParams params;
    params.radius = radius;
    params.height = (m_maxVertex.y - m_minVertex.y) * m_scale;
    params.restitution = restitution;
    params.friction = friction;
    params.density = density;
    params.initialPosition = btVector3(
        static_cast<btScalar>(position.x),
        static_cast<btScalar>(position.y + params.height / 2.0),    // makes center bottom the origin
        static_cast<btScalar>(position.z));
    btQuaternion q;
    q.setRotation(btVector3(0.0,0.0,1.0),0.0);
    params.initialRotation = q; 
    
    // update the model matrix (so origin of model is center bottom of puck)
    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(fRadius, fRadius, fRadius)) * m_modelMatrix;
    
    // this is the matrix to which all rotations and translations are applied
    m_centerScaleMatrix = m_modelMatrix;

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

    // don't allow the puck to deactivate
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);

    // keep puck from moving in the Y direction at all
    m_rigidBody->setLinearFactor(btVector3(1,0,1));

    // puck can't rotation around x or z axis
    m_rigidBody->setAngularFactor(btVector3(0,1,0));

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

