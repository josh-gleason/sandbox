#ifndef MODEL_HPP
#define MODEL_HPP

#include "iGLRenderable.hpp"

#include "GLAttribute.hpp"
#include "GLUniform.hpp"
#include "GLVertexArray.hpp"
#include "GLBuffer.hpp"
#include "GLTexture.hpp"

#include <boost/filesystem.hpp>
#include <assimp/scene.h>

namespace bf = boost::filesystem;

struct Material
{
    std::string name;
    glm::vec3 specular;
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 emissive;
    glm::vec3 transparent;
    float shininess;

    std::vector<bool>      hasTexture;
    std::vector<GLTexture> texture;
    std::vector<GLenum>    texTarget;
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

    bool init(const std::string& filename, GLAttribute& vPosition, GLAttribute& vNormal, GLAttribute& vUvCoord, const GLUniform& color);
    const glm::mat4& getModelMatrix() const;
    void loadMaterialTextures(int materialIdx, const aiMaterial& material);
    void draw();
    void centerScaleModel();
protected:
    static bool isWire(const std::string& name);
    void loadMaterials(aiMaterial** materials, unsigned int numMaterials);
    void loadMeshes(aiMesh** meshes, unsigned int numMeshes, GLAttribute& vPosition, GLAttribute& vNormal, GLAttribute& vUvCoord);
    void loadVertices(const aiMesh& mesh, GLsizei bufferIdx);
    void loadFaces(aiFace* faces, unsigned int numFaces, GLsizei bufferIdx);
    void loadUvs(aiMesh &mesh, GLsizei bufferIdx);

    GLBuffer              m_vertexBuffer;
    GLUniform             m_color;

    // one vao per mesh (m_meshInfo.size() == number of meshes)
    GLVertexArray         m_vao;
    std::vector<Material> m_materials;
    std::vector<MeshInfo> m_meshInfo;

    // used to find the bounding box
    bool                  m_minMaxInit;
    glm::vec3             m_minVertex;
    glm::vec3             m_maxVertex;

    // the model matrix
    glm::mat4           m_modelMatrix;

    // store the base path
    bf::path            m_modelDir;
};

#endif // MODEL_HPP

