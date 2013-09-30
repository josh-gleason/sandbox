#ifndef MODEL_HPP
#define MODEL_HPP

#include "iGLRenderable.hpp"

#include "GLAttribute.hpp"
#include "GLUniform.hpp"
#include "GLVertexArray.hpp"
#include "GLBuffer.hpp"

#include <assimp/scene.h>

struct Material
{
    glm::vec3 specular;
    glm::vec3 diffuse;
    glm::vec3 ambient;
};

struct MeshInfo
{
    GLsizei numElements;    // number of elements (3 per triangle)
    size_t materialIdx;     // which material to use
};

class Model : public iGLRenderable
{
public:
    Model();
    ~Model();

    bool init(const std::string& filename, GLAttribute& vPosition, GLAttribute& vNormal, const GLUniform& color);
    const glm::mat4& getModelMatrix() const;
    void draw();
protected:
    void loadMaterials(aiMaterial** materials, unsigned int numMaterials);
    void loadMeshes(aiMesh** meshes, unsigned int numMeshes, GLAttribute& vPosition, GLAttribute& vNormal);
    void loadVertices(aiVector3D* positions, aiVector3D* normals, unsigned int numVertices, GLsizei bufferIdx);
    void loadFaces(aiFace* faces, unsigned int numFaces, GLsizei bufferIdx);

    std::string         m_filename;
    GLBuffer            m_vertexBuffer;
    GLUniform           m_color;

    // one vao per mesh (m_meshInfo.size() == number of meshes)
    GLVertexArray       m_vao;
    std::vector<Material> m_materials;
    std::vector<MeshInfo> m_meshInfo;

    glm::mat4           m_modelMatrix;
};

#endif // MODEL_HPP

