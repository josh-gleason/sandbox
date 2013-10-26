#ifndef MODEL_HPP
#define MODEL_HPP

#include "../interfaces/iGLRenderable.hpp"

#include "../glwrappers/GLAttribute.hpp"
#include "../glwrappers/GLUniform.hpp"
#include "../glwrappers/GLVertexArray.hpp"
#include "../glwrappers/GLBuffer.hpp"
#include "../glwrappers/GLTexture.hpp"

#include <boost/filesystem.hpp>
#include <assimp/scene.h>

namespace bf = boost::filesystem;

struct Material
{
    // material info
    std::string name;
    glm::vec3 specular;
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 emissive;
    glm::vec3 transparent;
    float     shininess;
    float     texBlend;

    // texture info
    DrawType    drawType;
    GLTexture   texture;
    GLenum      texTarget;
    bool        useTexture;
};

struct MeshInfo
{
    std::string name;
    GLsizei numElements;    // number of elements (3 per triangle)
    size_t materialIdx;     // which material to use
    bool useTexture;
    GLTexture   texture;    // usually texture stored in material, but sometimes not
};

class Model : public iGLRenderable
{
public:
    Model();
    ~Model();

    bool init(const std::string& filename, bool flipUvs = false);
    
    // inherited virtual functions
    virtual const glm::mat4& getModelMatrix();
    void draw(DrawType type);
    void setUniforms(GLBuffer& ubo, UniformType type = MATERIALS);
protected:
    void drawCommon(size_t idx);
    void centerScaleModel();
    
    static bool isWire(const std::string& name);
    void loadMaterialTextures(int materialIdx, const aiMaterial& material);
    void loadMaterials(aiMaterial** materials, unsigned int numMaterials);
    void loadMeshes(aiMesh** meshes, unsigned int numMeshes);
    void loadTangents(const aiMesh& mesh, GLsizei bufferIdx);
    void loadVertices(const aiMesh& mesh, GLsizei bufferIdx, bool firstQuery);
    void loadFaces(aiFace* faces, unsigned int numFaces, GLsizei bufferIdx);
    void loadUvs(aiMesh &mesh, GLsizei bufferIdx);

    // vertex buffer kept around so that buffers aren't deallocated
    GLBuffer              m_vertexBuffer;

    GLBuffer              m_materialUbo;
    GLBuffer              m_texBlendUbo;

    // one vao per mesh (m_meshInfo.size() == number of meshes)
    GLVertexArray         m_vao;
    std::vector<Material> m_materials;
    std::vector<MeshInfo> m_meshInfo;

    // used to find the bounding box
    bool                  m_minMaxInit;
    glm::vec3             m_minVertex;
    glm::vec3             m_maxVertex;
    float                 m_scale;

    // the model matrix
    glm::mat4           m_modelMatrix;

    // store the base path
    bf::path            m_modelDir;
};

#endif // MODEL_HPP

