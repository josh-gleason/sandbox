#ifndef PUCK_HPP
#define PUCK_HPP

#include "Model.hpp"
#include "../interfaces/iPhysicsObject.hpp"
#include "../bulletwrappers/PhysicsWorld.hpp"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

const std::string PUCK_MODEL = "models/Puck/Puck.obj";
const double PUCK_RESTITUTION = 0.95;
const double PUCK_FRICTION = 0.1;
const double PUCK_DENSITY = 1.0;

class Puck : public Model, public iPhysicsObject
{
public:
    Puck();
    ~Puck();

    bool init(const PhysicsWorld& world, const glm::vec3& position, double radius, double density = PUCK_DENSITY, double friction = PUCK_FRICTION, double restitution = PUCK_RESTITUTION, const std::string& filename = PUCK_MODEL, bool flipUvs = false);
   
    void updateTransform();
protected:
    void initPhysics();

    double m_radius;
    double m_height;
    double m_mass;
    double m_friction;
    // restition [0,1] : 1 = elastic, 0 = inelastic
    double m_restitution;
    glm::vec3 m_position;
    
    PhysicsWorld m_physicsWorld;
   
    std::shared_ptr<btDefaultMotionState>    m_motionState;
    std::shared_ptr<btCollisionShape>        m_collisionShape;
    std::shared_ptr<btRigidBody>             m_rigidBody;
    std::shared_ptr<btGeneric6DofConstraint> m_constraint;
    btTransform                              m_transform;
    btVector3                                m_inertia;
    
    glm::mat4   m_centerScaleMatrix;
};

#endif // PUCK_HPP

