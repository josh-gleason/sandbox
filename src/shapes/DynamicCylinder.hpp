#ifndef DYNAMICCYLINDER_HPP
#define DYNAMICCYLINDER_HPP

#include "../interfaces/iPhysicsObject.hpp"
#include "../bulletwrappers/PhysicsWorld.hpp"
#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btQuaternion.h>
#include <bullet/LinearMath/btScalar.h>
#include <bullet/LinearMath/btTransform.h>

class btDefaultMotionState;
class btCollisionShape;
class btRigidBody;
class btGeneric6DofConstraint;

class DynamicCylinder : public iPhysicsObject
{
    public:
        struct InitialParams
        {
            btScalar radius;
            btScalar height;
            btScalar density;
            btScalar friction;
            btScalar restitution; // 1 = elastic, 0 = inelastic

            btVector3 initialPosition;
            btQuaternion initialRotation;
        };

        DynamicCylinder();
        virtual ~DynamicCylinder();

        virtual void updateTransform();
        bool initPhysics(const PhysicsWorld& world, const InitialParams& params);
    protected:
        InitialParams m_cylinderParams;

        PhysicsWorld m_physicsWorld;
       
        std::shared_ptr<btDefaultMotionState>    m_motionState;
        std::shared_ptr<btCollisionShape>        m_collisionShape;
        std::shared_ptr<btRigidBody>             m_rigidBody;
        std::shared_ptr<btGeneric6DofConstraint> m_constraint;
        btTransform                              m_transform;
        btVector3                                m_inertia;
        btScalar                                 m_mass;
};

#endif // DYNAMICCYLINDER_HPP

