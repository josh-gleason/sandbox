#ifndef GLSEGMENT_HPP
#define GLSEGMENT_HPP

#include <GL/glew.h>

#include "GLBuffer.hpp"
#include <vector>

#include <glm/glm.hpp>

struct GLAttributes
{
    glm::vec4   vPosition;
    glm::vec4   vColor;
};

struct GLSegmentUniforms
{
    glm::vec3   vAmbient;
    glm::vec3   vSpecular;
    glm::vec3   vDiffuse;
};

template <class V>
class GLVertexList
{
public:
    GLVertexList();

    std::vector<V> m_vertices;
};

template <class U>
class GLUniformList
{
public:
    GLUniformList();

    std::vector<U> m_uniforms;
};

class GLSegment
{
public:
    GLSegment();
    ~GLSegment();

    void draw();
protected:
    void updateBuffer(size_t bufferIdx, std::vector<size_t>(idx));

    GLBuffer                            m_buffer;
    GLVertexList<GLAttributes>          m_vertices;
    GLUniformList<GLSegmentUniforms>    m_uniforms;
};

#endif // GLSEGMENT_HPP

