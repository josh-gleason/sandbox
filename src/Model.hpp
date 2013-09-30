#ifndef MODEL_HPP
#define MODEL_HPP

#include "iGLRenderable.hpp"

#include "GLAttribute.hpp"
#include "GLUniform.hpp"
#include "GLVertexArray.hpp"
#include "GLBuffer.hpp"

struct Material
{
    glm::vec3 specular;
    glm::vec3 diffuse;
    glm::vec3 ambient;
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
    std::string         m_filename;
    GLVertexArray       m_vao;
    GLBuffer            m_vertexBuffer;
    GLUniform           m_color;
   
    unsigned int        m_numVaos;

    std::vector<size_t>   m_numElements;
    std::vector<size_t>   m_materialIdx;
    std::vector<Material> m_materials;

    glm::mat4           m_modelMatrix;
};

#endif // MODEL_HPP

