#include "GLTexture.hpp"

//#include <glimg/Loaders.h>
#include <iostream>
#include <IL/il.h>

#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>

GLTexture::GLTexture() :
    m_texCount(nullptr),
    m_texParameters(nullptr)
{};

GLTexture::~GLTexture()
{
    // clean up
    if ( m_texParameters.use_count() == 1 && m_texParameters == nullptr )
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
        m_texCount = std::shared_ptr<GLsizei>(new GLsizei[n]);
        m_texParameters = boost::shared_array<TexParameters>(new TexParameters[n]);

        GLuint *textures = new GLuint[n];
        std::cout << "Generating textures..." << std::flush;
        glGenTextures(n, textures);
        std::cout << "Complete!" << std::endl;
        for ( GLsizei i = 0; i < n; ++i )
        {
            m_texParameters[i].textureId = textures[i];
            m_texParameters[i].target = GL_NONE;
        }
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

bool GLTexture::loadImageData(const char* filename, GLsizei idx, GLenum type, GLenum format, GLint internalFormat, GLint lod)
{
#if 0
    try {

        glimg::ImageSet* imgData = glimg::loaders::stb::LoadFromFile(filename);
       
        glimg::Dimensions dims = imgData->GetDimensions();
        GLsizei dimVals[] = {dims.width, dims.height, dims.depth};

        cv::Mat img(dims.width, dims.height, CV_8UC3);
        
        std::cout << "Creating CV Image..." << std::endl;

        std::cout << dims.width << ' ' << dims.height << ' ' << dims.depth << std::endl;

        for ( int row = 0; row < dims.height; ++row )
            for ( int col = 0; col < dims.width; ++col )
                for ( int channel = 0; channel < 3; ++channel )
                    img.at<uchar>(row, col, channel) = *((uchar*)(imgData->GetImageArray(lod)) + row * dims.width * 3 + col * 3 + channel);
        std::cout << img << std::endl;
        imshow("Image", img);
        imwrite("temp.jpg",img);
        cv::waitKey(0);
        return this->setData(imgData->GetImageArray(lod), dimVals, idx, type, format, internalFormat, lod);
    } catch ( glimg::loaders::stb::UnableToLoadException &e ) {
        std::cerr << e.what() << std::endl;
    } catch ( glimg::loaders::stb::StbLoaderException &e ) {
        std::cerr << e.what() << std::endl;
    }
    return false;
#else
    ILuint a = ilGenImage();
    ilBindImage(a);
    if ( ilLoadImage(filename) )
    {
        ILubyte* data = ilGetData();
        ILint width = ilGetInteger(IL_IMAGE_WIDTH);
        ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
        ILint depth = ilGetInteger(IL_IMAGE_DEPTH);
        GLsizei dims[] = {width, height, depth};
       
        std::cout << width << ' ' << height << ' ' << depth << std::endl;
        
        cv::Mat img = cv::Mat(dims[1], dims[0], CV_8UC3).clone();
       
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                cv::Vec3b d(data[(i*width + j)*3 + 0],
                            data[(i*width + j)*3 + 1],
                            data[(i*width + j)*3 + 2]);
                img.at<cv::Vec3b>(i,j) = d;
            }
        }

        imshow("Image", img);
        imwrite("temp.jpg",img);
        bool retVal = this->setData(data, dims, idx, type, format, internalFormat, lod);
        ilClearImage();
        return retVal;
    }
    return false;
#endif
}

