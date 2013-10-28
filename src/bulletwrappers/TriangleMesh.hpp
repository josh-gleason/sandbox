#ifndef TRIANGLEMESH_HPP
#define TRIANGLEMESH_HPP

#include <string>
#include <memory>

#include <bullet/LinearMath/btVector3.h>
#include <boost/shared_array.hpp>
#include <assimp/vector3.h>

class btCollisionShape;
class btTriangleIndexVertexArray;
struct btIndexedMesh;
class aiMesh;

class TriangleMesh
{
public:
    TriangleMesh();
    ~TriangleMesh();

    bool loadMesh(const std::string& filename, const btVector3& scale);
    btCollisionShape* getShape();
    void reset();
protected:
    typedef boost::shared_array<unsigned char> SharedData;
    typedef boost::shared_array<SharedData> SharedDataList;

    bool loadModelMesh(const aiMesh* mesh, unsigned int idx);

    std::shared_ptr<btCollisionShape>             m_shape;
    std::shared_ptr<btTriangleIndexVertexArray>   m_mesh;
    boost::shared_array<btIndexedMesh>            m_data;
    int                                           m_dataSize;
    aiVector3D                                    m_scale;
   
    // holds all the data used by the mesh
    SharedDataList m_vertexBase;
    SharedDataList m_indexBase;
};

#endif // TRIANGLEMESH_HPP

