#include "Table.hpp"

#include <glm/ext.hpp>

Table::Table()
{

}

Table::~Table()
{}

bool Table::init(const PhysicsWorld& world, const glm::vec3& position, double scale, double friction, double restitution, const std::string& modelFilename, const std::string& wallsFilename, bool flipUvs)
{
    if ( !Model::init(modelFilename, flipUvs) )
        return false;

    // update the model matrix with scaling and translation
    m_modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f),glm::vec3(scale,scale,scale)) * glm::translate(glm::mat4(1.0f), m_translate * -m_scale) * m_modelMatrix;

    m_scale *= scale;
    InitialParams physicsParams;
    physicsParams.filename = wallsFilename;
    physicsParams.friction = friction;
    physicsParams.position = btVector3(position.x, position.y, position.z);
    physicsParams.restitution = restitution;
    physicsParams.rotation = btQuaternion(0,0,0,1);
    physicsParams.scale = btVector3(m_scale,m_scale,m_scale);
   
    if ( !initPhysics(world, physicsParams) )
        return false;

    return true;
}

void Table::updateTransform()
{
    // it's static, don't need to do anything
}

bool Table::initPhysics(const PhysicsWorld& world, const InitialParams& params)
{
    return StaticMesh::initPhysics(world, params);

    // add any table specific physics here
}

