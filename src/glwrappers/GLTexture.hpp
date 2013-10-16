#ifndef GLTEXTURE_HPP
#define GLTEXTURE_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <boost/shared_array.hpp>

class GLTexture
{
public:
    GLTexture();
    ~GLTexture();

    GLTexture &operator=(const GLTexture& rhs);

// keep for debugging (needs #include opencv2/opencv.hpp)
#if 0
    void showTexture(GLsizei idx = 0)
    {
        GLint width, height, format;
        this->bind(GL_TEXTURE_2D, idx);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

        std::cout << "Width/Height : (" << width << ',' << height << ')' << std::endl;
        cv::Mat img;
        if ( format == GL_RGB || format == GL_BGR )
            img = cv::Mat(height, width, CV_8UC3);
        else
            img = cv::Mat(height, width, CV_8UC4);

        glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, (void*)(img.data));
        cv::flip(img,img,0);    // flip vertical
        cv::imshow("Texture", img);
        cv::waitKey(0);
        cv::destroyWindow("Texture");
    }
#endif

    bool generate(GLsizei n = 1);

    // Supported values for target
    // (1D)
    //   GL_TEXTURE_1D
    //   GL_PROXY_TEXTURE_1D
    // (2D)
    //   GL_TEXTURE_2D
    //   GL_TEXTURE_1D_ARRAY
    //   GL_TEXTURE_RECTANGLE
    //   GL_TEXTURE_CUBE_MAP_POSITIVE_X
    //   GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    //   GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    //   GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    //   GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    //   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    //   GL_PROXY_TEXTURE_2D
    //   GL_PROXY_TEXTURE_1D_ARRAY
    //   GL_PROXY_TEXTURE_RECTANGLE
    //   GL_PROXY_TEXTURE_CUBE_MAP
    // (3D)
    //   GL_TEXTURE_3D
    //   GL_TEXTURE_2D_ARRAY
    //   GL_TEXTURE_CUBE_MAP_ARRAY
    //   GL_PROXY_TEXTURE_3D
    //   GL_PROXY_TEXTURE_2D_ARRAY
    //   GL_PROXY_TEXTURE_CUBE_MAP_ARRAY
    void bind(GLenum target, GLsizei idx = 0);

    // T is the data type (ex. if type is GL_UNSIGNED_BYTE, T should be a GLubyte)
    // S is assumed to be an array of GLfloats with width, height, depth for dims=TEX_3D,
    //     width, height if dims=TEX_2D, and width if dims=TEX_1D
    // Data starts at lower left corner of back image and goes left to right, bottom to
    //     top, back to front.
    template <typename T>
    bool setData(const T* data, const GLsizei* dimVals, GLsizei idx = 0, GLenum type = GL_UNSIGNED_BYTE, GLenum format = GL_RGB, GLint internalFormat = GL_RGBA, GLint lod = 0 );

    bool loadImageData(const char* filename, GLsizei idx = 0, GLenum internaFormat = GL_RGBA, GLint lod = 0);
    void setSampling(GLenum target, GLenum minFilter = GL_NEAREST, GLenum magFilter = GL_NEAREST, GLenum wrapS = GL_WRAP_BORDER, GLenum wrapT = GL_WRAP_BORDER);
    void generateMipMap(GLenum target);

    static void unbindTextures(GLenum target);
protected:
    void clean();

    struct TexParameters
    {
        GLuint textureId;
        GLenum target;
        glm::vec3 dimensions; // width, height, depth
        GLenum type;
        GLenum format;
        GLint internalFormat;
        GLint lod;  // level-of-detail (0 = highest detail)
        size_t dataTypeSize;
    };

    std::shared_ptr<GLsizei> m_texCount;
    boost::shared_array<TexParameters> m_texParameters; 
};

template <typename T>
bool GLTexture::setData(const T* data, const GLsizei* dimVals, GLsizei idx, GLenum type, GLenum format, GLint internalFormat, GLint lod)
{
    if ( m_texParameters != nullptr && idx < *m_texCount && m_texParameters[idx].target != GL_NONE )
    {
        switch(m_texParameters[idx].target)
        {
            case GL_TEXTURE_1D:
            case GL_PROXY_TEXTURE_1D:
                glTexImage1D(m_texParameters[idx].target,
                    lod,
                    internalFormat,
                    dimVals[0], // width
                    0,
                    format,
                    type,
                    reinterpret_cast<const void*>(data));
                break;
            case GL_TEXTURE_2D:
            case GL_TEXTURE_1D_ARRAY:
            case GL_TEXTURE_RECTANGLE:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            case GL_PROXY_TEXTURE_2D:
            case GL_PROXY_TEXTURE_1D_ARRAY:
            case GL_PROXY_TEXTURE_RECTANGLE:
            case GL_PROXY_TEXTURE_CUBE_MAP:
                glTexImage2D(m_texParameters[idx].target,
                    lod,
                    internalFormat,
                    dimVals[0], // width
                    dimVals[1], // height
                    0,
                    format,
                    type,
                    reinterpret_cast<const void*>(data));
                break;
            case GL_TEXTURE_3D:
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            case GL_PROXY_TEXTURE_3D:
            case GL_PROXY_TEXTURE_2D_ARRAY:
            case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
                glTexImage3D(m_texParameters[idx].target,
                    lod,
                    internalFormat,
                    dimVals[0], // width
                    dimVals[1], // height
                    dimVals[2], // depth
                    0,
                    format,
                    type,
                    reinterpret_cast<const void*>(data));
                break;
            default:
                return false;
        }
    }
    return true;
}

#endif // GLTEXTURE_HPP

