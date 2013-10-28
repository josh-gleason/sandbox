#ifndef TABLE_HPP
#define TABLE_HPP

#include "Model.hpp"
#include "StaticMesh.hpp"

const double TABLE_FRICTION = 1.0;
const double TABLE_RESTITUTION = 1.0;
const double TABLE_LENGTH = 5.0;
const std::string TABLE_MODEL = "models/AirHockeyTable/Airhockey.obj";
const std::string TABLE_WALLS = "models/AirHockeyTable/walls/walls2.obj";

class Table : public Model, public StaticMesh
{
public:
    Table();
    virtual ~Table();

    bool init(const PhysicsWorld& world, const glm::vec3& position, double scale = TABLE_LENGTH/2.0, double friction = TABLE_FRICTION, double restitution = TABLE_RESTITUTION, const std::string& modelFilename = TABLE_MODEL, const std::string& wallsFilename = TABLE_WALLS, bool flipUvs = false);
   
    void updateTransform();
protected:
    bool initPhysics(const PhysicsWorld& world, const InitialParams& params);
};

#endif // TABLE_HPP

