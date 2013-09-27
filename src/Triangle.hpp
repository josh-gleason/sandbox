#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "iGLRenderable.hpp"
#include "GLProgram.hpp"
#include "GLBuffer.hpp"

#include <glm/glm.hpp>

#include <iostream>
using namespace std;

class Triangle : public iGLRenderable
{
public:
    Triangle()
    {

    }

    ~Triangle()
    {

    }

    void init(const GLProgram& program, const GLchar* positionAttribName, const GLchar* colorAttribName)
    {
        glm::vec3 verts[] = {
            glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(1.0f,0.0f,0.0f),
            glm::vec3(-0.9f, -0.9f, 0.0f), glm::vec3(0.0f,1.0f,0.0f),
            glm::vec3(0.9f, -0.9f, 0.0f), glm::vec3(0.0f,0.0f,1.0f)
        };

        GLubyte idx[] = {0,1,2};

        m_program = program;

        m_vertexBuffer.generate(2);

        m_vertexBuffer.bind(GL_ARRAY_BUFFER, 0);
        m_vertexBuffer.setData<glm::vec3>(verts, sizeof(verts)/sizeof(glm::vec3), GL_STATIC_DRAW, 0);
        m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
        
        m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, 1);
        m_vertexBuffer.setData<GLubyte>(idx, sizeof(idx)/sizeof(GLubyte), GL_STATIC_DRAW, 1);
        m_vertexBuffer.unbindBuffers(GL_ELEMENT_ARRAY_BUFFER);

        m_locVerts = m_program.getAttributeLocation(positionAttribName);
        cout << m_program.getLastError() << endl;
        m_locColors = m_program.getAttributeLocation(colorAttribName);
        cout << m_program.getLastError() << endl;
    }

    void draw()
    {
        m_program.use();
        glEnableVertexAttribArray(m_locVerts);
        glEnableVertexAttribArray(m_locColors);
        m_vertexBuffer.bind(GL_ARRAY_BUFFER, 0);
        m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, 1);
        
        glVertexAttribPointer(m_locVerts, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)*2, 0);
        glVertexAttribPointer(m_locColors, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)*2, (void*)(sizeof(glm::vec3)));

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (void*)(0));
    
        m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
        m_vertexBuffer.unbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
        
        m_program.resetUsed();
    }

protected:
    GLBuffer  m_vertexBuffer;
    GLint     m_locVerts;
    GLint     m_locColors;
    GLProgram m_program;
};

#endif // TRIANGLE_HPP

