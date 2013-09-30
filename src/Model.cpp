#include "Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <boost/shared_array.hpp>

Model::Model() :
    m_filename(""),
    m_modelMatrix(1.0)
{}

Model::~Model()
{}

void Model::loadMaterials(aiMaterial** materials, unsigned int numMaterials)
{
    // resize the material matrix
    m_materials.resize(numMaterials);

    for ( unsigned int i = 0; i < numMaterials; ++i )
    {
        aiMaterial &material = *(materials[i]);

        aiColor3D ambient(0.0f, 0.0f, 0.0f);
        aiColor3D diffuse(0.0f, 0.0f, 0.0f);
        aiColor3D specular(0.0f, 0.0f, 0.0f);

        material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material.Get(AI_MATKEY_COLOR_SPECULAR, specular);

        m_materials[i].diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        m_materials[i].ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        m_materials[i].specular = glm::vec3(specular.r, specular.g, specular.b);
    }
}

void Model::loadVertices(aiVector3D* positions, aiVector3D* normals, unsigned int numVertices, GLsizei bufferIdx)
{
    // interleave the positions and normals
    glm::vec3 *verticesGlm = new glm::vec3[numVertices*2];

    // copy positions and normals into single, interleaved array
    for ( unsigned int i = 0; i < numVertices*2; i+=2 )
    {
        verticesGlm[i] = glm::vec3(positions[i/2].x, positions[i/2].y, positions[i/2].z);
        verticesGlm[i+1] = glm::vec3(normals[i/2].x, normals[i/2].y, normals[i/2].z);
    }

    // load vertices into an array buffer
    m_vertexBuffer.bind(GL_ARRAY_BUFFER, bufferIdx);
    m_vertexBuffer.setData<glm::vec3>(verticesGlm, numVertices*2, GL_STATIC_DRAW, 0);
    m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
    delete [] verticesGlm;
}

void Model::loadFaces(aiFace* faces, unsigned int numFaces, GLsizei bufferIdx)
{
    // guarenteed 3 elements per face
    unsigned int numElements = numFaces * 3;

    // using GLuints because meshes may be very large (FYI Ui just means unsigned int)
    GLuint *facesUi = new GLuint[numElements];

    // copy elements from faces to facesUi
    for ( unsigned int j = 0; j < numElements; ++j )
        facesUi[j] = faces[j/3].mIndices[j%3];

    // load elements into buffer
    m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, bufferIdx);
    m_vertexBuffer.setData<GLuint>(facesUi, numFaces*3, GL_STATIC_DRAW, 0);
    m_vertexBuffer.unbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
    delete [] facesUi;
}

void Model::loadMeshes(aiMesh** meshes, unsigned int numMeshes, GLAttribute& vPosition, GLAttribute& vNormal)
{
    unsigned int vertexBufferIdx;
    unsigned int elementBufferIdx;

    // resize the mesh information vectors
    m_meshInfo.resize(numMeshes);

    // one VAO for each mesh
    m_vao.create(numMeshes);

    // generate 2 buffers for each mesh, 1 for vertex/normals (interleaved) and 1 for the element array
    m_vertexBuffer.generate(numMeshes * 2);

    // copy vertices into meshes
    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        // provide easier access to *(meshes[i])
        aiMesh& mesh = *(meshes[i]);

        vertexBufferIdx = 2*i;
        elementBufferIdx = 2*i + 1;

        // enable the VAO for this mesh and enable the necessary attributes
        // using braces/indentation to show what code applies to this vao
        m_vao.bind(i);
        {
            // Load vertices and faces into buffers
            this->loadVertices(mesh.mVertices, mesh.mNormals, mesh.mNumVertices, vertexBufferIdx);
            this->loadFaces(mesh.mFaces, mesh.mNumFaces, elementBufferIdx);

            // enable the attributes for the VAO
            vPosition.enable();
            vNormal.enable();

            // bind and point data to the array buffer. The 3 is because vec3 is 3
            // elements (GL_FLOAT implied). Step size is 2*sizeof(vec3) because the
            // data is interleaved. The normals has an offset of sizeof(vec3) because
            // of interleaving.
            m_vertexBuffer.bind(GL_ARRAY_BUFFER, vertexBufferIdx);
            vPosition.loadBufferData(3, sizeof(glm::vec3)*2);
            vNormal.loadBufferData(3, sizeof(glm::vec3)*2, sizeof(glm::vec3));

            // bind the element array buffer for the VAO
            m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, elementBufferIdx);
        }
        // unbind the VAO so that no further changes will affect it
        m_vao.unbindAll();

        // store the mesh info
        m_meshInfo[i].materialIdx = mesh.mMaterialIndex;
        m_meshInfo[i].numElements = 3 * mesh.mNumFaces;
    }
}

bool Model::init(const std::string& filename, GLAttribute &vPosition, GLAttribute &vNormal, const GLUniform& color)
{
    m_color = color;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( filename,
        aiProcess_GenNormals               |    // gen normals if lacking
        aiProcess_Triangulate              |    // only get triangles
        aiProcess_JoinIdenticalVertices    |    // save memory
        aiProcess_SortByPType);                 // ?

    if (!scene)
        return false;

    // copy the materials over
    this->loadMaterials(scene->mMaterials, scene->mNumMaterials);
    this->loadMeshes(scene->mMeshes, scene->mNumMeshes, vPosition, vNormal);

    // initialize the model matrix
    m_modelMatrix = glm::mat4(1.0f);

    return true;
}

const glm::mat4& Model::getModelMatrix() const
{
    return m_modelMatrix;
}

void Model::draw()
{
    for ( unsigned int i = 0; i < m_meshInfo.size(); ++i )
    {
        m_color.loadData(m_materials[m_meshInfo[i].materialIdx].diffuse);
        m_color.set();
        
        m_vao.bind(i);
        glDrawElements(GL_TRIANGLES, m_meshInfo[i].numElements, GL_UNSIGNED_INT, (void*)(0));
    }
    m_vao.unbindAll();
}

