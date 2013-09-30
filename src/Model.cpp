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

bool Model::init(const std::string& filename, GLAttribute &vPosition, GLAttribute &vNormal, const GLUniform& color)
{
    m_color = color;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( filename,
        //aiProcess_CalcTangentSpace         |
        //aiProcess_GenSmoothNormals         |    // gen smooth normals
        aiProcess_GenNormals               |    // gen normals if lacking
        aiProcess_Triangulate              |    // only get triangles
        aiProcess_JoinIdenticalVertices    |    // save memory
        aiProcess_SortByPType);                 // ?

    if (!scene)
        return false;

    // generate enough buffers
    // 1 buffer for each vertex for each mesh
    // 1 element array buffer for each mesh (faces)
    m_vertexBuffer.generate(scene->mNumMeshes * 2);
    m_vao.create(scene->mNumMeshes);

    unsigned int numMeshes = scene->mNumMeshes;
    unsigned int numMaterials = scene->mNumMaterials;

    m_numVaos = numMeshes;
    m_materialIdx.resize(numMeshes);
    m_materials.resize(numMaterials);
    m_numElements.resize(numMeshes);

    // copy the materials over
    for ( unsigned int i = 0; i < numMaterials; ++i )
    {
        aiMaterial &material = *(scene->mMaterials[i]);

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

    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        // get all this in a vao
        m_vao.bind(i);
        aiMesh& mesh = *(scene->mMeshes[i]);
        
        // Load in vertices
        unsigned int numVerts = mesh.mNumVertices;
        aiVector3D *position = mesh.mVertices;
        aiVector3D *normal = mesh.mNormals;

        // interleave the positions and normals
        glm::vec3 *verticesGlm = new glm::vec3[numVerts*2];

        for ( unsigned int j = 0; j < numVerts*2; j+=2 )
        {
            verticesGlm[j] = glm::vec3(position[j/2].x, position[j/2].y, position[j/2].z);
            verticesGlm[j+1] = glm::vec3(normal[j/2].x, normal[j/2].y, normal[j/2].z);
        }

        m_vertexBuffer.bind(GL_ARRAY_BUFFER, i*2);
        m_vertexBuffer.setData<glm::vec3>(verticesGlm, numVerts*2, GL_STATIC_DRAW, 0);
        m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
        delete [] verticesGlm;

        // Load in Faces
        unsigned int numFaces = mesh.mNumFaces;
        aiFace *faces = mesh.mFaces;
        GLuint *facesUi = new GLuint[numFaces*3];   // guarenteed 3 elements per face
        for ( unsigned int j = 0; j < numFaces*3; ++j )
            facesUi[j] = faces[j/3].mIndices[j%3];

        m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, i*2+1);
        m_vertexBuffer.setData<GLuint>(facesUi, numFaces*3, GL_STATIC_DRAW, 0);
        m_vertexBuffer.unbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
        delete [] facesUi;
        
        m_numElements[i] = numFaces*3;

        // bind the vertex buffer
        m_vertexBuffer.bind(GL_ARRAY_BUFFER, i);

        // set the attribute pointers
        vPosition.enable();
        vNormal.enable();

        vPosition.loadBufferData(3, sizeof(glm::vec3)*2);
        vNormal.loadBufferData(3, sizeof(glm::vec3)*2, sizeof(glm::vec3));

        // the element array buffer MUST be bound before unbinding the VAO
        m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, i+1);

        // unbind the vao
        m_vao.unbindAll();
        
        // save the material index
        m_materialIdx[i] = mesh.mMaterialIndex;
    }

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
    for ( unsigned int i = 0; i < m_numVaos; ++i )
    {
        m_color.loadData(m_materials[m_materialIdx[i]].diffuse);
        m_color.set();
        
        m_vao.bind(i);
        glDrawElements(GL_TRIANGLES, m_numElements[i], GL_UNSIGNED_INT, (void*)(0));
    }
    m_vao.unbindAll();
}

