#ifndef PUCK_HPP
#define PUCK_HPP

#include "Model.hpp"
#include "DynamicCylinder.hpp"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

const std::string PUCK_MODEL = "models/Puck/Puck.obj";
const double PUCK_RESTITUTION = 0.95;
const double PUCK_FRICTION = 0.1;
const double PUCK_DENSITY = 1.0;

class Puck : public Model, public DynamicCylinder 
{
public:
    Puck();
    virtual ~Puck();

    bool init(const PhysicsWorld& world, const glm::vec3& position, double radius, double density = PUCK_DENSITY, double friction = PUCK_FRICTION, double restitution = PUCK_RESTITUTION, const std::string& filename = PUCK_MODEL, bool flipUvs = false);
   
    void updateTransform();
protected:
    bool initPhysics(const PhysicsWorld& world, const InitialParams& params);

    glm::mat4   m_centerScaleMatrix;
};

#endif // PUCK_HPP

