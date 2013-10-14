#include "Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/shared_array.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define CHECKERR err = glGetError(); if ( err != GL_NO_ERROR ) { if ( err == GL_INVALID_OPERATION ) std::cout << "Error: INVALID_OPERATION Line " << __LINE__ << std::endl; else if (err == GL_INVALID_VALUE) std::cout << "Error: INVALID_VALUE Line " << __LINE__ << std::endl; else std::cout << "Error: Line " << __LINE__ << std::endl; }

// same order as TEXTURE_TYPES defined in iGLRenderable
const aiTextureType AI_TEXTURE_TYPES[] = {aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS};
const int TEXTURE_TYPE_COUNT = sizeof(AI_TEXTURE_TYPES)/sizeof(aiTextureType);

Model::Model() :
    m_minMaxInit(false),
    m_modelMatrix(1.0)
{}

Model::~Model()
{}

void Model::centerScaleModel()
{
    if ( !m_minMaxInit )
        return;

    float scale = 2.0f / std::max(m_maxVertex.x - m_minVertex.x,
                         std::max(m_maxVertex.y - m_minVertex.y,
                                  m_maxVertex.z - m_minVertex.z));

    glm::vec3 center = (m_maxVertex + m_minVertex) / 2.0f;
    glm::vec3 translate = glm::vec3(-center.x, -m_minVertex.y, -center.z);

    m_modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale,scale,scale)),translate);
}

void Model::loadMaterialTextures(int materialIdx, const aiMaterial& material)
{
    m_materials[materialIdx].drawType = DRAW_MATERIAL;
    m_materials[materialIdx].texture.resize(TEXTURE_TYPE_COUNT);
    m_materials[materialIdx].texTarget.resize(TEXTURE_TYPE_COUNT);

    for ( int i = 0; i < TEXTURE_TYPE_COUNT; ++i )
    {
        std::cout << "i : " << i << std::endl;
        aiString texImg("");
        aiReturn queryResult;
        
        queryResult = material.Get(AI_MATKEY_TEXTURE(AI_TEXTURE_TYPES[i], 0), texImg);

        if ( queryResult == AI_SUCCESS )
        {
            switch (AI_TEXTURE_TYPES[i])
            {
                case aiTextureType_DIFFUSE:
                    std::cout << "Found diffuse" << std::endl;
                    break;
                case aiTextureType_SPECULAR:
                    std::cout << "Found specular" << std::endl;
                    break;
                case aiTextureType_NORMALS:
                    std::cout << "Found bump" << std::endl;
                    break;
                default:
                    std::cout << "Found unknown" << std::endl;
            }

            m_materials[materialIdx].drawType =
                static_cast<DrawType>(static_cast<unsigned int>(m_materials[materialIdx].drawType) | TEXTURE_TYPES[i]);

            // activate texture i
            glActiveTexture(GL_TEXTURE0 + i);

            // create local path to texture
            bf::path texPath = m_modelDir / texImg.C_Str();

            // generate a texture
            GLTexture tex2d;
            tex2d.generate(1);
            tex2d.bind(GL_TEXTURE_2D);
            
            // load the texture into opengl
            if ( !tex2d.loadImageData(texPath.c_str()) )
            {
                // try this path too
                bf::path texPath = m_modelDir / "Texture" / texImg.C_Str();
                if ( !tex2d.loadImageData(texPath.c_str()) )
                {
                    std::cout << "Unable to load texture \"" << texImg.C_Str()
                              << "\"" << std::endl;
                    // disable bit
                    m_materials[materialIdx].drawType =
                        static_cast<DrawType>(static_cast<unsigned int>(m_materials[materialIdx].drawType) & ~(TEXTURE_TYPES[i]));
                }
            }

            // save the texture
            if ( (m_materials[materialIdx].drawType & TEXTURE_TYPES[i]) == TEXTURE_TYPES[i] )
            {
                std::cout << "Using texture type : " << i << std::endl;
                m_materials[materialIdx].useTexture = true;
                tex2d.generateMipMap(GL_TEXTURE_2D);
                m_materials[materialIdx].texture[i] = tex2d;
                m_materials[materialIdx].texTarget[i] = GL_TEXTURE_2D;
            }
           
            tex2d.unbindTextures(GL_TEXTURE_2D);
        }
        else
        {
            switch (AI_TEXTURE_TYPES[i])
            {
                case aiTextureType_DIFFUSE:
                    std::cout << "NOT FOUND diffuse" << std::endl;
                    break;
                case aiTextureType_SPECULAR:
                    std::cout << "NOT FOUND specular" << std::endl;
                    break;
                case aiTextureType_NORMALS:
                    std::cout << "NOT FOUND bump" << std::endl;
                    break;
                default:
                    std::cout << "NOT FOUND unknown" << std::endl;
            }
        }
    }
    std::cout << "FINAL TEXTURE TYPE : " << m_materials[materialIdx].drawType << std::endl;

    // reset active texture
    glActiveTexture(GL_TEXTURE0);
}

void Model::loadMaterials(aiMaterial** materials, unsigned int numMaterials)
{
    // resize the material matrix
    m_materials.resize(numMaterials);

    for ( unsigned int i = 0; i < numMaterials; ++i )
    {
        aiMaterial &material = *(materials[i]);

        aiString name;
        aiColor3D ambient(0.2f, 0.2f, 0.2f);
        aiColor3D diffuse(1.0f, 0.5f, 0.5f);
        aiColor3D specular(0.3f, 0.3f, 0.3f);
        aiColor3D reflective(0.0f, 0.0f, 0.0f);
        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        aiColor3D transparent(0.0f, 0.0f, 0.0f);
        m_materials[i].shininess = 20.0;

        material.Get(AI_MATKEY_NAME, name);
        material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material.Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material.Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        material.Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
        material.Get(AI_MATKEY_SHININESS, m_materials[i].shininess);
        
        m_materials[i].texBlend = 1.0f;
        m_materials[i].name = name.C_Str();
        m_materials[i].diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        m_materials[i].ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        m_materials[i].specular = glm::vec3(specular.r, specular.g, specular.b);
        m_materials[i].emissive = glm::vec3(emissive.r, emissive.g, emissive.b);
        m_materials[i].transparent = glm::vec3(transparent.r, transparent.g, transparent.b);

        m_materials[i].drawType = DRAW_MATERIAL;
//        this->loadMaterialTextures(i, material);
    }
}

void Model::loadVertices(const aiMesh& mesh, GLsizei bufferIdx)
{
    const aiVector3D* positions = mesh.mVertices;
    const aiVector3D* normals = mesh.mNormals;
    unsigned int numVertices = mesh.mNumVertices;

    // interleave the positions and normals
    GLfloat *vertices = new GLfloat[numVertices*6];

    // copy positions and normals into single, interleaved array
    for ( unsigned int i = 0; i < numVertices*6; i+=6 )
    {
        vertices[i]   = positions[i/6].x;
        vertices[i+1] = positions[i/6].y;
        vertices[i+2] = positions[i/6].z;
        vertices[i+3] = normals[i/6].x;
        vertices[i+4] = normals[i/6].y;
        vertices[i+5] = normals[i/6].z;
        if ( !m_minMaxInit )
        {
            m_minMaxInit = true;
            m_minVertex =
            m_maxVertex = glm::vec3(positions[i/6].x, positions[i/6].y, positions[i/6].z);
        }
        else
        {
            m_minVertex = glm::vec3(
                std::min(vertices[i  ], m_minVertex.x),
                std::min(vertices[i+1], m_minVertex.y),
                std::min(vertices[i+2], m_minVertex.z));
            m_maxVertex = glm::vec3(
                std::max(vertices[i  ], m_maxVertex.x),
                std::max(vertices[i+1], m_maxVertex.y),
                std::max(vertices[i+2], m_maxVertex.z));
        }
    }

    // load vertices into an array buffer
    m_vertexBuffer.bind(GL_ARRAY_BUFFER, bufferIdx);
    m_vertexBuffer.setData<GLfloat>(vertices, numVertices*6, GL_STATIC_DRAW, 0);
    m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
    delete [] vertices;
}

void Model::loadUvs(aiMesh &mesh, GLsizei bufferIdx)
{
    if ( mesh.mTextureCoords[0] == nullptr ) return;

    unsigned int numVertices = mesh.mNumVertices;

    GLfloat *uvCoords = new GLfloat[numVertices*2];

    for ( unsigned int i = 0; i < numVertices*2; i+=2 )
    {
        uvCoords[i] = mesh.mTextureCoords[0][i/2].x;
        uvCoords[i+1] = mesh.mTextureCoords[0][i/2].y;
    }

    m_vertexBuffer.bind(GL_ARRAY_BUFFER, bufferIdx);
    m_vertexBuffer.setData<GLfloat>(uvCoords, numVertices*2, GL_STATIC_DRAW, 0);
    m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
    delete [] uvCoords;
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
    m_vertexBuffer.setData(facesUi, numFaces*3, GL_STATIC_DRAW, bufferIdx);
    m_vertexBuffer.unbindBuffers(GL_ELEMENT_ARRAY_BUFFER);
    delete [] facesUi;
}

void Model::loadTangents(const aiMesh &mesh, GLsizei bufferIdx)
{
    const aiVector3D* tangents = mesh.mTangents;
    const aiVector3D* binormals = mesh.mBitangents;
    unsigned int numVertices = mesh.mNumVertices;

    // interleave the positions and normals
    GLfloat *vertices = new GLfloat[numVertices*6];

    // copy positions and normals into single, interleaved array
    for ( unsigned int i = 0; i < numVertices*6; i+=6 )
    {
        vertices[i]   = tangents[i/6].x;
        vertices[i+1] = tangents[i/6].y;
        vertices[i+2] = tangents[i/6].z;
        vertices[i+3] = binormals[i/6].x;
        vertices[i+4] = binormals[i/6].y;
        vertices[i+5] = binormals[i/6].z;
    }

    // load vertices into an array buffer
    m_vertexBuffer.bind(GL_ARRAY_BUFFER, bufferIdx);
    m_vertexBuffer.setData<GLfloat>(vertices, numVertices*6, GL_STATIC_DRAW, bufferIdx);
    m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
    delete [] vertices;
}

void Model::loadMeshes(aiMesh** meshes, unsigned int numMeshes, GLAttribute& vPosition, GLAttribute& vNormal)
//void Model::loadMeshes(aiMesh** meshes, unsigned int numMeshes, GLAttribute& vPosition, GLAttribute& vNormal, GLAttribute& vTangent, GLAttribute& vBinormal, GLAttribute& vUvCoord)
{
    unsigned int vertexBufferIdx;
    unsigned int uvBufferIdx;
    unsigned int tangentBufferIdx;
    unsigned int elementBufferIdx;
    unsigned int nextBuffer = 0;

    // resize the mesh information vectors
    m_meshInfo.resize(numMeshes);

    // one VAO for each mesh
    m_vao.create(numMeshes);

    unsigned int bufferCount = 0;
    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        aiMesh& mesh = *(meshes[i]);
        
        bool hasTexture = m_materials[mesh.mMaterialIndex].useTexture;
        
        if ( hasTexture )
            bufferCount+=2; // tangents and uvs
        bufferCount+=2;
    }

    // generate 2 buffers for each mesh, 1 for vertex/normals (interleaved) and 1 for the element array
    m_vertexBuffer.generate(bufferCount);

    // copy vertices into meshes
    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        // provide easier access to *(meshes[i])
        aiMesh& mesh = *(meshes[i]);
        
        bool hasTexture = m_materials[mesh.mMaterialIndex].useTexture;

        vertexBufferIdx = nextBuffer++;
        elementBufferIdx = nextBuffer++;
        if ( hasTexture )
        {
            uvBufferIdx = nextBuffer++;
            tangentBufferIdx = nextBuffer++;
        }

        // enable the VAO for this mesh and enable the necessary attributes
        // using braces/indentation to show what code applies to this vao
        m_vao.bind(i);
        {
            // Load vertices and faces into buffers
            this->loadVertices(mesh, vertexBufferIdx);
            this->loadFaces(mesh.mFaces, mesh.mNumFaces, elementBufferIdx);
            if ( hasTexture )
            {
//                this->loadUvs(mesh, uvBufferIdx);
//                this->loadTangents(mesh, tangentBufferIdx);
//                vUvCoord.enable();
//                vTangent.enable();
//                vBinormal.enable();
            }

            // enable the attributes for the VAO
            vPosition.enable();
            vNormal.enable();

            // bind and point data to the array buffer. The 3 is because vec3 is 3
            // elements (GL_FLOAT implied). Step size is 2*sizeof(vec3) because the
            // data is interleaved. The normals has an offset of sizeof(vec3) because
            // of interleaving.
            m_vertexBuffer.bind(GL_ARRAY_BUFFER, vertexBufferIdx);
            vPosition.loadBufferData(3, sizeof(GLfloat)*6);
            vNormal.loadBufferData(3, sizeof(GLfloat)*6, sizeof(GLfloat)*3);
            if ( hasTexture )
            {
                // TODO (is this needed? )
//                m_vertexBuffer.unbindBuffers(GL_ARRAY_BUFFER);
//                m_vertexBuffer.bind(GL_ARRAY_BUFFER, uvBufferIdx);
//                vUvCoord.loadBufferData(2, sizeof(GLfloat)*2);

                // load tangents and binormals
//                m_vertexBuffer.bind(GL_ARRAY_BUFFER, tangentBufferIdx);
//                vTangent.loadBufferData(3, sizeof(GLfloat)*6);
//                vBinormal.loadBufferData(3, sizeof(GLfloat)*6, sizeof(GLfloat)*3);
            }

            // bind the element array buffer for the VAO
            m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, elementBufferIdx);
        }
        // unbind the VAO so that no further changes will affect it
        m_vao.unbindAll();
        
        // store material idx and number of faces
        m_meshInfo[i].materialIdx = mesh.mMaterialIndex;
        m_meshInfo[i].numElements = 3 * mesh.mNumFaces;
    }
}

bool Model::init(const std::string& filename, GLAttribute& vPosition, GLAttribute& vNormal)
//bool Model::init(const std::string& filename, GLAttribute& vPosition, GLAttribute& vNormal, GLAttribute& vTangent, GLAttribute &vBinormal, GLAttribute& vUvCoord)
{
    m_modelDir = bf::path(filename).remove_filename();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( filename,// aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenSmoothNormals );
        aiProcess_GenSmoothNormals         |
        aiProcess_CalcTangentSpace         |
        aiProcess_GenUVCoords              |
        aiProcess_Triangulate              |    // only get triangles
        aiProcess_JoinIdenticalVertices    |    // save memory
        aiProcess_SortByPType              |    // This and the next one ignore points/lines
        aiProcess_FindDegenerates);             // Remove bad triangles (share one or more points)

    if (!scene)
        return false;

    m_minMaxInit = false;
    // copy the materials over
    this->loadMaterials(scene->mMaterials, scene->mNumMaterials);
    this->loadMeshes(scene->mMeshes, scene->mNumMeshes, vPosition, vNormal);
    //this->loadMeshes(scene->mMeshes, scene->mNumMeshes, vPosition, vNormal, vTangent, vBinormal, vUvCoord);

    // initialize the model matrix
    this->centerScaleModel();
    return true;
}

const glm::mat4& Model::getModelMatrix() const
{
    return m_modelMatrix;
}

bool Model::isWire(const std::string& name)
{
    if ( name.size() >= 4 )
    {
        return ( name[0] == 'w' &&
                 name[1] == 'i' &&
                 name[2] == 'r' &&
                 name[3] == 'e' );
    }
    return false;
}

void Model::setUniforms(GLBuffer &ubo, UniformType type)
{
    switch (type)
    {
        case UniformType::MATERIALS:
            m_materialUbo = ubo;
            break;
        default:
            std::cout << "Warning: type not handled" << std::endl;
    }
}

void Model::draw(DrawType type)
{
    switch (type)
    {
        case DrawType::DRAW_MATERIAL:
        {
            for ( size_t i = 0; i < m_meshInfo.size(); ++i )
                if ( m_materials[m_meshInfo[i].materialIdx].drawType == DRAW_MATERIAL )
                    this->drawCommon(i);
            break;
        }
        default:
        {
            std::cout << "Warning: Draw type not handeled" << std::endl;
        }
    }
}

void Model::drawCommon(size_t idx)
{
    if ( this->isWire(m_materials[m_meshInfo[idx].materialIdx].name) )
        return;
  
    // set the materials UBO
    m_materialUbo.bind(GL_UNIFORM_BUFFER);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].diffuse), MATERIAL_DIFFUSE_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].specular), MATERIAL_SPECULAR_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].ambient), MATERIAL_AMBIENT_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].shininess), MATERIAL_SHININESS_OFFSET);
   
    GLBuffer::unbindBuffers(GL_UNIFORM_BUFFER);

    m_vao.bind(idx);
    glDrawElements(GL_TRIANGLES, m_meshInfo[idx].numElements, GL_UNSIGNED_INT, (void*)(0));
    m_vao.unbindAll();
}

