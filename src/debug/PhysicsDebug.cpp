#include "PhysicsDebug.hpp"

#include <glm/glm.hpp>

PhysicsDebug::PhysicsDebug() :
    m_modelMatrix(1.0f)
{
    m_vao = std::shared_ptr<GLVertexArray>(new GLVertexArray);
    m_vao->create(1);
}

PhysicsDebug::~PhysicsDebug()
{}

const glm::mat4& PhysicsDebug::getModelMatrix()
{
    return m_modelMatrix;
}

void PhysicsDebug::draw(DrawType type)
{
    if ( type == DrawType::DRAW_MATERIAL )
    {
        m_vao->bind();
        glDrawArrays(GL_LINES, 0, m_numVertices);
        m_vao->unbindAll();
    }
}

void PhysicsDebug::loadToBuffer()
{
    // attributes
    GLAttribute vPosition(V_POSITION);
    GLAttribute vColor(V_COLOR);

    // initialize buffers
    m_vertexBuffer = std::shared_ptr<GLBuffer>(new GLBuffer);
    m_colorBuffer = std::shared_ptr<GLBuffer>(new GLBuffer);
    m_vertexBuffer->generate(1);
    m_colorBuffer->generate(1);

    m_vao->bind();

    // bind vertex locations
    m_vertexBuffer->bind(GL_ARRAY_BUFFER);
    m_vertexBuffer->setData(m_vertices.data(), m_vertices.size(), GL_DYNAMIC_DRAW);
    vPosition.enable();
    vPosition.loadBufferData(3, sizeof(glm::vec3));

    // bind vertex colors
    m_colorBuffer->bind(GL_ARRAY_BUFFER);
    m_colorBuffer->setData(m_colors.data(), m_colors.size(), GL_DYNAMIC_DRAW);
    vColor.enable();
    vColor.loadBufferData(3, sizeof(glm::vec3));

    m_vao->unbindAll();

    m_numVertices = m_vertices.size();

    // clear the vertices for the next time
    m_vertices.clear();
    m_colors.clear();
}

void PhysicsDebug::setUniforms(GLBuffer&, UniformType)
{
    // no uniforms to set
}

void PhysicsDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_vertices.push_back(glm::vec3(from.x(), from.y(), from.z()));
    m_vertices.push_back(glm::vec3(to.x(), to.y(), to.z()));
    m_colors.push_back(glm::vec3(color.x(), color.y(), color.z()));
    m_colors.push_back(glm::vec3(color.x(), color.y(), color.z()));
}

