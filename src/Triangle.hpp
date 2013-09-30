#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "iGLRenderable.hpp"
#include "GLProgram.hpp"
#include "GLBuffer.hpp"
#include "GLVertexArray.hpp"
#include "GLAttribute.hpp"
#include "GLUniform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

class Triangle : public iGLRenderable
{
public:
    Triangle()
    {}

    ~Triangle()
    {}

    void updateModel(const glm::mat4& modelMatrix)
    {
        m_modelMatrix = modelMatrix;
    }

    const glm::mat4& getModelMatrix() const
    {
        return m_modelMatrix;
    }

    void init(const GLProgram& program, const GLchar* positionAttribName, const GLchar* colorAttribName)
    {
        glm::vec3 verts[] = {
            glm::vec3(0.0f, 0.9f, 1.0f), glm::vec3(1.0f,0.0f,0.0f),
            glm::vec3(-0.9f, -0.9f, 1.0f), glm::vec3(0.0f,1.0f,0.0f),
            glm::vec3(0.9f, -0.9f, 1.0f), glm::vec3(0.0f,0.0f,1.0f)
        };
        GLubyte idx[] = {0,1,2};

        // copy program to member variables and use it
        m_program = program;
        
        // initialize model uniform matrix
        this->updateModel(glm::mat4(1.0f));

        // get attribute locations
        m_vPositionAttrib.init(m_program, positionAttribName);
        m_vColorAttrib.init(m_program, colorAttribName);
        
        // generate two buffers
        m_vertexBuffer.generate(2);

        // create new vertex array object and bind it. All buffer/attribute operations are stored
        // in the VAO until it is unbound. This lets us draw later by just binding the VAO
        m_vao.create();
        m_vao.bind();
            
            // set array buffer data
            m_vertexBuffer.bind(GL_ARRAY_BUFFER, 0);
            m_vertexBuffer.setData<glm::vec3>(verts, sizeof(verts)/sizeof(glm::vec3), GL_STATIC_DRAW, 0);
            m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);

            // set vertex indexing data
            m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, 1);
            m_vertexBuffer.setData<GLubyte>(idx, sizeof(idx)/sizeof(GLubyte), GL_STATIC_DRAW, 1);
            m_vertexBuffer.unbindBuffers(GL_ELEMENT_ARRAY_BUFFER);

            // enable attributes
            m_vPositionAttrib.enable();
            m_vColorAttrib.enable();

            // bind the vertex buffer
            m_vertexBuffer.bind(GL_ARRAY_BUFFER, 0);

            // set vertex attributes to know how to access the array buffer data
            m_vPositionAttrib.loadBufferData(3, sizeof(glm::vec3)*2); 
            m_vColorAttrib.loadBufferData(3, sizeof(glm::vec3)*2, sizeof(glm::vec3));
            
            // bind the element array buffer for DrawElements to know which vertices to draw
            m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, 1);
            
        // set back to default vertex array
        m_vao.unbindAll();
    }

    void draw()
    {
        // bind the VAO and draw using the indicies in element array buffer
        m_vao.bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (void*)(0));
        m_vao.unbindAll();
    }

protected:
    GLBuffer       m_vertexBuffer;
    GLAttribute    m_vPositionAttrib;
    GLAttribute    m_vColorAttrib;
    GLVertexArray  m_vao;
    GLProgram      m_program;
    glm::mat4      m_modelMatrix;
};

#endif // TRIANGLE_HPP

