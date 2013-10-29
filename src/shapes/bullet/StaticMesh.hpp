#ifndef STATICMESH_HPP
#define STATICMESH_HPP

#include "../../interfaces/iPhysicsObject.hpp"
#include "../../bulletwrappers/TriangleMesh.hpp"
#include "../../bulletwrappers/PhysicsWorld.hpp"

#include <bullet/LinearMath/btQuaternion.h>
#include <bullet/LinearMath/btVector3.h>

#include <memory>

struct btDefaultMotionState;
class btCollisionShape;

class StaticMesh : public iPhysicsObject
{
public:
    struct InitialParams
    {
        std::string  filename;
        btVector3    position;
        btVector3    scale;
        btQuaternion rotation;
        btScalar     friction;
        btScalar     restitution; // 1 = elastic, 0 = inelastic
    };

    StaticMesh();
    virtual ~StaticMesh();

    virtual void updateTransform();
    virtual bool initPhysics(const PhysicsWorld& world, const InitialParams& params);
    void reset();
protected:
    InitialParams m_meshParams;

    PhysicsWorld m_physicsWorld;

    TriangleMesh                          m_mesh;
    std::shared_ptr<btDefaultMotionState> m_motionState;
    std::shared_ptr<btRigidBody>          m_rigidBody;
};

#endif // STATICMESH_HPP

