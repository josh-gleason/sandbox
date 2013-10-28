#include "TriangleMesh.hpp"

#include <bullet/btBulletCollisionCommon.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

TriangleMesh::TriangleMesh()
{}

TriangleMesh::~TriangleMesh()
{
    this->reset();
}

bool TriangleMesh::loadModelMesh(const aiMesh* mesh, unsigned int idx)
{
    unsigned int numFaces = mesh->mNumFaces;
    unsigned int numVertices = mesh->mNumVertices;

    const aiFace *faces = mesh->mFaces;
    const aiVector3D *vertices = mesh->mVertices;

    // set info for bullet
    m_data[idx].m_indexType = PHY_ScalarType::PHY_INTEGER;
    m_data[idx].m_vertexType = PHY_ScalarType::PHY_FLOAT;
    m_data[idx].m_vertexStride = sizeof(float)*3;
    m_data[idx].m_triangleIndexStride = sizeof(int)*3;
    m_data[idx].m_numVertices = numVertices;
    m_data[idx].m_numTriangles = numFaces;

    // create list to use for triangle mesh
    m_vertexBase[idx] = SharedData(new unsigned char[m_data[idx].m_vertexStride * numVertices]);
    m_indexBase[idx] = SharedData(new unsigned char[m_data[idx].m_triangleIndexStride * numFaces]);

    // some fun with memory!
    // copy vertices (and scale)
    for ( unsigned int i = 0; i < numVertices; ++i )
    {
        *reinterpret_cast<float*>(m_vertexBase[idx].get() + i*m_data[idx].m_vertexStride)                   = m_scale.x * vertices[i].x;
        *reinterpret_cast<float*>(m_vertexBase[idx].get() + i*m_data[idx].m_vertexStride + sizeof(float))   = m_scale.y * vertices[i].y;
        *reinterpret_cast<float*>(m_vertexBase[idx].get() + i*m_data[idx].m_vertexStride + sizeof(float)*2) = m_scale.z * vertices[i].z;
    }
    // copy triangle indicies
    for ( unsigned int i = 0; i < numFaces; ++i )
    {
        if ( faces->mNumIndices != 3U )
            return false;
        memcpy(reinterpret_cast<void*>(m_indexBase[idx].get() + i*m_data[idx].m_triangleIndexStride),
               reinterpret_cast<void*>(faces[i].mIndices),
               static_cast<size_t>(m_data[idx].m_triangleIndexStride));
    }

    // set data to parameters used by bullet
    m_data[idx].m_triangleIndexBase = m_indexBase[idx].get();
    m_data[idx].m_vertexBase = m_vertexBase[idx].get();

    return true;
}

bool TriangleMesh::loadMesh(const std::string& filename, const btVector3& scale)
{
    m_mesh = std::shared_ptr<btTriangleMesh>(new btTriangleMesh());

    Assimp::Importer importer;

    // these values used with aiProcess_RemoveComponent
    // the following components aren't loaded (only meshes are loaded)
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
            aiComponent_NORMALS |
            aiComponent_TEXTURES |
            aiComponent_MATERIALS |
            aiComponent_BONEWEIGHTS |
            aiComponent_ANIMATIONS |
            aiComponent_LIGHTS |
            aiComponent_CAMERAS |
            aiComponent_TEXCOORDS |
            aiComponent_TANGENTS_AND_BITANGENTS |
            aiComponent_COLORS);

    // don't load lines or points
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
            aiPrimitiveType_POINT |
            aiPrimitiveType_LINE |
            aiPrimitiveType_POLYGON
    );

    // import the model
    // TODO : debug output write it to log (see aiProcess_ValidateDataStructure flag)
    const aiScene* scene = importer.ReadFile(filename,
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_ImproveCacheLocality |
            aiProcess_FindDegenerates |
            aiProcess_SortByPType |
            aiProcess_RemoveComponent |
            aiProcess_OptimizeMeshes |
            aiProcess_OptimizeGraph
    );
    
    if (!scene)
        return false;
    
    // initialize new data
    m_dataSize = scene->mNumMeshes;
    m_data = boost::shared_array<btIndexedMesh>(new btIndexedMesh[m_dataSize]);
    m_vertexBase = SharedDataList(new SharedData[m_dataSize]);
    m_indexBase = SharedDataList(new SharedData[m_dataSize]);
    m_scale = aiVector3D(scale.x(), scale.y(), scale.z());

    // load in mesh
    for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
    {
        if ( !this->loadModelMesh(scene->mMeshes[i], i) )
        {
            this->reset();
            return false;
        }
        m_mesh->addIndexedMesh(m_data[i], PHY_ScalarType::PHY_INTEGER);
    } 

    // create collision shape 
    m_shape = std::shared_ptr<btCollisionShape>(new btBvhTriangleMeshShape(m_mesh.get(), true));

    return true;
}

btCollisionShape* TriangleMesh::getShape()
{
    return m_shape.get();
}

void TriangleMesh::reset()
{
    m_shape.reset();
    m_mesh.reset();
    m_vertexBase.reset();
    m_indexBase.reset();
    m_dataSize = 0;
}

