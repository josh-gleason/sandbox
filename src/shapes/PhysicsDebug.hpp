#ifndef DEBUGDRAW_HPP
#define DEBUGDRAW_HPP

#include <bullet/LinearMath/btIDebugDraw.h>
#include "../interfaces/iGLRenderable.hpp"
#include "../glwrappers/GLVertexArray.hpp"
#include "../glwrappers/GLAttribute.hpp"

class PhysicsDebug : public btIDebugDraw, public iGLRenderable
{
public:
    PhysicsDebug();
    ~PhysicsDebug();

    // This will be called
    const glm::mat4& getModelMatrix();
    void draw(DrawType type = DRAW_MATERIAL);
    void setUniforms(GLBuffer &ubo, UniformType type = MATERIALS);

    // overloaded pure virtual from btIDebugDraw
    void loadToBuffer();
    void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

    void drawContactPoint(const btVector3&,const btVector3&,btScalar,int,const btVector3&) {}
    void reportErrorWarning(const char*) {}
    void draw3dText(const btVector3&,const char*) {}
    void setDebugMode(int) {}
    int getDebugMode() const { return 0; }
    
    // originals with argument names
    //void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {}
    //void reportErrorWarning(const char* warningString) {}
    //void draw3dText(const btVector3& location,const char* textString) {}
    //void setDebugMode(int debugMode) {}
protected:
    GLsizei                         m_numVertices;
    
    std::shared_ptr<GLBuffer>       m_vertexBuffer;
    std::shared_ptr<GLBuffer>       m_colorBuffer;
    std::shared_ptr<GLVertexArray>  m_vao;

    std::vector<glm::vec3>          m_vertices;
    std::vector<glm::vec3>          m_colors;
    
    // always identity
    glm::mat4                       m_modelMatrix;
};

#endif // DEBUGDRAW_HPP

