#include "Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/shared_array.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <cstring>

#define CHECKERR err = glGetError(); if ( err != GL_NO_ERROR ) { if ( err == GL_INVALID_OPERATION ) std::cout << "Error: INVALID_OPERATION Line " << __LINE__ << std::endl; else if (err == GL_INVALID_VALUE) std::cout << "Error: INVALID_VALUE Line " << __LINE__ << std::endl; else std::cout << "Error: Line " << __LINE__ << std::endl; }

// same order as TEXTURE_TYPES defined in iGLRenderable
const aiTextureType AI_TEXTURE_TYPES[] = {aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS};

const int TEXTURE_TYPE_COUNT = 1; // Just checking for DIFFUSE textures for now 
//const int TEXTURE_TYPE_COUNT = sizeof(AI_TEXTURE_TYPES)/sizeof(aiTextureType);

#ifdef MESSAGES_DEBUG
    #define DEBUG_MSG(a) a
#else
    #define DEBUG_MSG(a) //
#endif

Model::Model() :
    m_minMaxInit(false),
    m_scale(1.0),
    m_modelMatrix(1.0)
{}

Model::~Model()
{}

void Model::centerScaleModel()
{
    m_scale = 2.0f / std::max(m_maxVertex.x - m_minVertex.x,
                     std::max(m_maxVertex.y - m_minVertex.y,
                              m_maxVertex.z - m_minVertex.z));

    glm::vec3 center = (m_maxVertex + m_minVertex) / 2.0f;
    m_translate = glm::vec3(-center.x, -center.y, -center.z);

    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(m_scale,m_scale,m_scale)) * glm::translate(glm::mat4(1.0f),m_translate);
}

void Model::loadMaterialTextures(int materialIdx, const aiMaterial& material)
{
    // default value is to have no textures
    m_materials[materialIdx].drawType = DRAW_MATERIAL;

    aiString texImg("");
    aiReturn queryResult =
        material.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texImg);

    DEBUG_MSG(std::cout << "Material Index " << materialIdx << std::endl);

    if ( queryResult == AI_SUCCESS )
    {
        DEBUG_MSG(std::cout << "Found Diffuse texture" << std::endl);

        // set draw type to include bit representing diffuse texture
        m_materials[materialIdx].drawType =
            static_cast<DrawType>(static_cast<unsigned int>(m_materials[materialIdx].drawType) | TEXTURE_DIFFUSE);

        // activate texture 0 for the diffuse texture
        glActiveTexture(GL_TEXTURE0);

        // create local path to texture
        bf::path texPath = m_modelDir / texImg.C_Str();

        // generate a texture (will be copied to array once sure it is valid)
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
                DEBUG_MSG(std::cout << "Unable to load texture \"" << texImg.C_Str()
                          << "\"" << std::endl);

                // we were wrong, no diffuse texture so disable the bit
                m_materials[materialIdx].drawType =
                    static_cast<DrawType>(static_cast<unsigned int>(m_materials[materialIdx].drawType) & (~TEXTURE_DIFFUSE));
            }
        }

        // if texture is valid save the texture to the Material list
        if ( (m_materials[materialIdx].drawType & TEXTURE_DIFFUSE) == TEXTURE_DIFFUSE )
        {
            // use textures
            m_materials[materialIdx].useTexture = true;

            // generate mip maps to make rendering less intense
            tex2d.generateMipMap(GL_TEXTURE_2D);
            m_materials[materialIdx].texture = tex2d;
            m_materials[materialIdx].texTarget = GL_TEXTURE_2D;
        }
       
        tex2d.unbindTextures(GL_TEXTURE_2D);
    }
    else // no textures
    {
        DEBUG_MSG(std::cout << "Diffuse texture not found with material" << std::endl);
    }
    DEBUG_MSG(std::cout << "FINAL TEXTURE TYPE : " << m_materials[materialIdx].drawType << std::endl);

    // reset active texture to default
    glActiveTexture(GL_TEXTURE0);
}

void Model::loadMaterials(aiMaterial** materials, unsigned int numMaterials)
{
    // resize the material matrix
    m_materials.resize(numMaterials);

    for ( unsigned int i = 0; i < numMaterials; ++i )
    {
        aiMaterial &material = *(materials[i]);

        // default values for texture
        aiString name(std::string("none"));
        aiColor3D ambient(0.2f, 0.2f, 0.2f);
        aiColor3D diffuse(1.0f, 0.5f, 0.5f);
        aiColor3D specular(0.3f, 0.3f, 0.3f);
        aiColor3D reflective(0.0f, 0.0f, 0.0f);
        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        aiColor3D transparent(0.0f, 0.0f, 0.0f);
        m_materials[i].shininess = 20.0;
        m_materials[i].texBlend = 1.0f;

        material.Get(AI_MATKEY_NAME, name);
        material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material.Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material.Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        material.Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
        material.Get(AI_MATKEY_SHININESS, m_materials[i].shininess);
        material.Get(AI_MATKEY_TEXBLEND_DIFFUSE(0), m_materials[i].texBlend);
       
        // sometimes the ambient is all 1s making things washed out
        if ( ambient.r > 0.99 && ambient.g > 0.99 && ambient.b > 0.99 )
            ambient.r = ambient.g = ambient.b = 0.1;

        m_materials[i].name = name.C_Str();
        m_materials[i].diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        m_materials[i].ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        m_materials[i].specular = glm::vec3(specular.r, specular.g, specular.b);
        m_materials[i].emissive = glm::vec3(emissive.r, emissive.g, emissive.b);
        m_materials[i].transparent = glm::vec3(transparent.r, transparent.g, transparent.b);

        // set texture info
        this->loadMaterialTextures(i, material);
    }
}

void Model::loadVertices(const aiMesh& mesh, GLsizei bufferIdx, bool firstQuery)
{
    const aiVector3D* positions = mesh.mVertices;
    const aiVector3D* normals = mesh.mNormals;
    unsigned int numVertices = mesh.mNumVertices;

    // interleave the positions and normals
    GLfloat *vertices = new GLfloat[numVertices*6];
   
    if ( firstQuery )
        m_minVertex = m_maxVertex =
            glm::vec3(positions[0].x, positions[0].y, positions[0].z);

    // copy positions and normals into single, interleaved array
    for ( unsigned int i = 0; i < numVertices*6; i+=6 )
    {
        if ( positions != nullptr )
        {
            vertices[i]   = positions[i/6].x;
            vertices[i+1] = positions[i/6].y;
            vertices[i+2] = positions[i/6].z;
        }
        else
        {
            vertices[i] = vertices[i+1] = vertices[i+2] = 0.0;
        }

        if ( normals != nullptr )
        {
            vertices[i+3] = normals[i/6].x;
            vertices[i+4] = normals[i/6].y;
            vertices[i+5] = normals[i/6].z;
        }
        else
        {
            vertices[i+3] = vertices[i+4] = 0.0;
            vertices[i+5] = 1.0;
        }

        // while loading, determine bounding box
        m_minVertex = glm::vec3(
            std::min(vertices[i  ], m_minVertex.x),
            std::min(vertices[i+1], m_minVertex.y),
            std::min(vertices[i+2], m_minVertex.z));
        m_maxVertex = glm::vec3(
            std::max(vertices[i  ], m_maxVertex.x),
            std::max(vertices[i+1], m_maxVertex.y),
            std::max(vertices[i+2], m_maxVertex.z));
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

void Model::loadMeshes(aiMesh** meshes, unsigned int numMeshes)
{
    unsigned int vertexBufferIdx;
    unsigned int uvBufferIdx;
    unsigned int elementBufferIdx;
    unsigned int nextBuffer = 0;
    bool initMinMaxSearch = true;

    // attributes
    GLAttribute vPosition(V_POSITION);
    GLAttribute vNormal(V_NORMAL);
    GLAttribute vUvCoord(V_UVCOORD);

    // resize the mesh information vectors
    m_meshInfo.resize(numMeshes);

    // one VAO for each mesh
    m_vao.create(numMeshes);

    // determine number of buffers needed
    unsigned int bufferCount = 0;
    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        aiMesh& mesh = *(meshes[i]);
        m_meshInfo[i].name = mesh.mName.C_Str();
        
        DEBUG_MSG(std::cout << "Mesh : " << m_meshInfo[i].name << std::endl);

        if ( m_materials[mesh.mMaterialIndex].drawType != DRAW_MATERIAL )
            bufferCount++; // uv buffer
        bufferCount+=2; // vertex and element buffers
    }

    // generate the required number of buffers
    m_vertexBuffer.generate(bufferCount);

    // copy vertices into meshes
    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        // provide easier access to *(meshes[i])
        aiMesh& mesh = *(meshes[i]);
        
        bool useTexture = m_materials[mesh.mMaterialIndex].useTexture; 

        vertexBufferIdx = nextBuffer++;
        elementBufferIdx = nextBuffer++;
        if ( useTexture )
            uvBufferIdx = nextBuffer++;

        // enable the VAO for this mesh and enable the necessary attributes
        // using braces/indentation to show what code applies to this vao
        m_vao.bind(i);
        {
            // Load vertices and faces into buffers
            this->loadVertices(mesh, vertexBufferIdx, initMinMaxSearch);
            this->loadFaces(mesh.mFaces, mesh.mNumFaces, elementBufferIdx);
            if ( useTexture )
            {
                this->loadUvs(mesh, uvBufferIdx);
                vUvCoord.enable();
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
            if ( useTexture )
            {
                vUvCoord.enable();
                m_vertexBuffer.bind(GL_ARRAY_BUFFER, uvBufferIdx);
                vUvCoord.loadBufferData(2, sizeof(GLfloat)*2);
            }

            // bind the element array buffer for the VAO
            // (note here that GL_ARRAY_BUFFERS are NOT stored by the VAO) but attribute enables are
            m_vertexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER, elementBufferIdx);
        }
        // unbind the VAO so that no further changes will affect it
        m_vao.unbindAll();

        // store material idx and number of faces
        m_meshInfo[i].materialIdx = mesh.mMaterialIndex;
        m_meshInfo[i].numElements = 3 * mesh.mNumFaces;

        // only valid first time through the loop
        initMinMaxSearch = false;
    }
}

bool Model::init(const std::string& filename, bool flipUvs)
{
    m_modelDir = bf::path(filename).remove_filename();

    Assimp::Importer importer;

    // don't load these components (used with aiProcess_RemoveComponent)
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
            aiComponent_BONEWEIGHTS |
            aiComponent_ANIMATIONS |
            aiComponent_LIGHTS |
            aiComponent_CAMERAS
    );
    // don't process points and lines
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
            aiPrimitiveType_POINT |
            aiPrimitiveType_LINE |
            aiPrimitiveType_POLYGON // should be triangulated but just in case...
    );
    
    // import the model
    // TODO : debug output write it to log (see aiProcess_ValidateDataStructure flag)
    const aiScene* scene =
        importer.ReadFile( filename,
                aiProcess_RemoveComponent |
                aiProcessPreset_TargetRealtime_MaxQuality |
                (flipUvs ? 0 : aiProcess_FlipUVs)
        );

    if (!scene)
        return false;

    // flag used to tell loadMeshes that no computations have been done
    this->loadMaterials(scene->mMaterials, scene->mNumMaterials);
    this->loadMeshes(scene->mMeshes, scene->mNumMeshes);
    // initialize the model matrix
    this->centerScaleModel();
    return true;
}

const glm::mat4& Model::getModelMatrix()
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
        case UniformType::TEXBLEND:
            m_texBlendUbo = ubo;
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
        case DrawType::DRAW_TEXTURE_D:  // Diffuse texture only
        {
            for ( size_t i = 0; i < m_meshInfo.size(); ++i )
            {
                Material& material = m_materials[m_meshInfo[i].materialIdx];

                if ( material.drawType != DRAW_TEXTURE_D )
                    continue;
                
                // bind the correct texture
                glActiveTexture(GL_TEXTURE0);
                material.texture.setSampling(material.texTarget,
                        GL_LINEAR_MIPMAP_LINEAR,
                        GL_LINEAR,
                        GL_MIRRORED_REPEAT,
                        GL_MIRRORED_REPEAT);
                material.texture.generateMipMap(material.texTarget);
                material.texture.bind(material.texTarget);
                
                this->drawCommon(i);
            }
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
    // don't draw wires/bones in models
    //if ( this->isWire(m_materials[m_meshInfo[idx].materialIdx].name) )
    //    return;

#if 1
    // make elexis nude
    std::string name = m_materials[m_meshInfo[idx].materialIdx].name;
    if ( name == "NudeEL_Nude__Elexis_reference_s0" )
        return;
    if ( name == "NudeEL_Nude__Elexis_reference_s" )
        return;
#endif

    // set the materials UBO
    m_materialUbo.bind(GL_UNIFORM_BUFFER);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].diffuse), MATERIAL_DIFFUSE_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].specular), MATERIAL_SPECULAR_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].ambient), MATERIAL_AMBIENT_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].shininess), MATERIAL_SHININESS_OFFSET);
    m_materialUbo.setSubData(&(m_materials[m_meshInfo[idx].materialIdx].texBlend), MATERIAL_TEXBLEND_OFFSET);
    GLBuffer::unbindBuffers(GL_UNIFORM_BUFFER);

    m_vao.bind(idx);
    glDrawElements(GL_TRIANGLES, m_meshInfo[idx].numElements, GL_UNSIGNED_INT, (void*)(0));
    m_vao.unbindAll();
}

