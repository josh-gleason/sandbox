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
    std::string name;
    glm::vec3 specular;
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 emissive;
    glm::vec3 transparent;
    float shininess;
    float texBlend;

    std::vector<bool>      hasTexture;
    std::vector<GLTexture> texture;
    std::vector<GLenum>    texTarget;
    bool useTexture;
};

struct MeshInfo
{
    GLsizei numElements;    // number of elements (3 per triangle)
    size_t materialIdx;     // which material to use
};

struct Uniforms
{
    GLUniform uDiffuse;
    GLUniform uSpecular;
    GLUniform uAmbient;
    GLUniform uShininess;
    GLUniform uTexBlend;
};

class Model : public iGLRenderable
{
public:
    Model();
    ~Model();

    bool init(const std::string& filename, GLAttribute& vPosition, GLAttribute& vNormal, GLAttribute &vTangent, GLAttribute &vBinormal, GLAttribute &vUvCoord);
    void centerScaleModel();
    
    // inherited virtual functions
    const glm::mat4& getModelMatrix() const;
    void draw(DrawType type);
    //void setUniforms(GLUniform *uniforms[], DrawType type);
    void setUniforms(const std::vector<GLUniform>& uniforms, DrawType type = DRAW_MATERIAL);
    void drawTextured();
protected:
    void drawCommon(size_t idx);

    static bool isWire(const std::string& name);
    void loadMaterialTextures(int materialIdx, const aiMaterial& material);
    void loadMaterials(aiMaterial** materials, unsigned int numMaterials);
    void loadMeshes(aiMesh** meshes, unsigned int numMeshes, GLAttribute& vPosition, GLAttribute& vNormal, GLAttribute& vTangent, GLAttribute& vBinormal, GLAttribute& vUvCoord);
    void loadTangents(const aiMesh& mesh, GLsizei bufferIdx);
    void loadVertices(const aiMesh& mesh, GLsizei bufferIdx);
    void loadFaces(aiFace* faces, unsigned int numFaces, GLsizei bufferIdx);
    void loadUvs(aiMesh &mesh, GLsizei bufferIdx);

    GLBuffer              m_vertexBuffer;
    Uniforms              m_uniforms;

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

