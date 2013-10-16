#include "GLTexture.hpp"

#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>

GLTexture::GLTexture() :
    m_texCount(nullptr),
    m_texParameters(nullptr)
{};

GLTexture::~GLTexture()
{
}
    
GLTexture& GLTexture::operator=(const GLTexture& rhs)
{
    // clean up
    this->clean();

    m_texParameters = rhs.m_texParameters;
    m_texCount = rhs.m_texCount;

    return *this;
}


void GLTexture::clean()
{
    // clean up
    if ( m_texParameters != nullptr && m_texParameters.use_count() == 1 && m_texCount != nullptr)
    {
        GLuint *textures = new GLuint[*m_texCount];
        for ( GLsizei i = 0; i < *m_texCount; ++i )
            textures[i] = m_texParameters[i].textureId;

        glDeleteTextures( *m_texCount, textures );
        delete [] textures;
    }
}

bool GLTexture::generate(GLsizei n)
{
    if ( n > 0 )
    {
        this->clean();

        m_texCount = std::shared_ptr<GLsizei>(new GLsizei);
        m_texParameters = boost::shared_array<TexParameters>(new TexParameters[n]);
      
        GLuint *textures = new GLuint[n];
        glGenTextures(n, textures);
        for ( GLsizei i = 0; i < n; ++i )
        {
            m_texParameters[i].textureId = textures[i];
            m_texParameters[i].target = GL_NONE;
        }
        
        // This line! OMG so many problems because it was forgotten!
        *m_texCount = n;        

        delete [] textures;
        return true;
    }
    return false;
}

void GLTexture::bind(GLenum target, GLsizei idx)
{
    if ( m_texParameters != nullptr && idx < *m_texCount )
    {
        glBindTexture(target, m_texParameters[idx].textureId);
        m_texParameters[idx].target = target;
    }
}

void GLTexture::unbindTextures(GLenum target)
{
    glBindTexture(target, 0);
}

void GLTexture::setSampling(GLenum target, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT)
{
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
}

void GLTexture::generateMipMap(GLenum target)
{
    glGenerateMipmap(target);
}

bool GLTexture::loadImageData(const char* filename, GLsizei idx, GLenum internalFormat, GLint lod)
{
    bool retVal = false;
    if ( ilLoadImage(filename) )
    {
        // flip image in necessary
        ILinfo ImageInfo;
        iluGetImageInfo(&ImageInfo);
        if( ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT )
            iluFlipImage();

        // get image information
        ILubyte* data = ilGetData();
        ILint width = ilGetInteger(IL_IMAGE_WIDTH);
        ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
        ILint depth = ilGetInteger(IL_IMAGE_DEPTH);
        GLenum type = static_cast<GLenum>(ilGetInteger(IL_IMAGE_TYPE));
        GLenum format = static_cast<GLenum>(ilGetInteger(IL_IMAGE_FORMAT));
        GLsizei dims[] = {width, height, depth};

        // set the data to the texture
        retVal = this->setData(data, dims, idx, type, format, internalFormat, lod);

        // clean up
        ilClearImage();
    }
    return retVal;
}

