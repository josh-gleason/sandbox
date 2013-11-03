#ifndef GLUNIFORM_HPP
#define GLUNIFORM_HPP

#include <GL/glew.h>
#include <string>

#include "GLProgram.hpp"

// lists the types of uniforms
enum GLUniformType {
    INT_V, UINT_V, FLOAT_V, DOUBLE_V,
    VEC2UI_V, VEC3UI_V, VEC4UI_V,
    VEC2I_V, VEC3I_V, VEC4I_V,
    VEC2F_V, VEC3F_V, VEC4F_V,
    VEC2D_V, VEC3D_V, VEC4D_V,
    INT, UINT, FLOAT, DOUBLE,
    VEC2UI, VEC3UI, VEC4UI,
    VEC2I, VEC3I, VEC4I,
    VEC2F, VEC3F, VEC4F,
    VEC2D, VEC3D, VEC4D,
    MAT2F, MAT3F, MAT4F, MAT2X3F, MAT3X2F, MAT2X4F, MAT4X2F, MAT3X4F, MAT4X3F,
    MAT2D, MAT3D, MAT4D, MAT2X3D, MAT3X2D, MAT2X4D, MAT4X2D, MAT3X4D, MAT4X3D
};

// example if T is glm::mat4 then setFunc should be glProgramUniform4f
class GLUniform
{
public:
    GLUniform();
    GLUniform(GLProgram program, const std::basic_string<GLchar>& name, GLUniformType type);
    GLUniform(const GLUniform &rhs);
    ~GLUniform();
    GLUniform& operator=(const GLUniform& rhs);

    // must be called before using others
    bool init(GLProgram program, const std::basic_string<GLchar>& name, GLUniformType type);

    // load data into uniform, doesn't load into opengl yet (call set for that)
    template <typename T>
    void loadData(const T &data, GLint count = 1, GLboolean transpose = GL_FALSE)
    {
        if ( m_data != nullptr )
            delete [] m_data;

        m_size = sizeof(T);
        m_count = count;
        m_transpose = transpose;

        m_data = new char[count*m_size];

        // copy data
        const char *dataPtr = reinterpret_cast<const char*>(&data);
        for ( int i = 0; i < m_count*m_size; ++i )
            m_data[i] = dataPtr[i];
    }

    // use to get the data array or data
    template <typename T>
    const T* getDataArray() const
    {
        return reinterpret_cast<T*>(m_data);
    }
    
    template <typename T>
    const T& getData() const
    {
        return *reinterpret_cast<T*>(m_data);
    }

    // call this to load uniform into opengl
    void set();

    // accessors
    GLboolean isTransposed() const { return m_transpose; }
    GLUniformType getType() const { return m_type; }
    int getLength() const { return m_count; }
    const std::basic_string<GLchar>& getName() const { return m_name; }

    // check if the uniform has been initialized
    bool isInitialized() const { return m_isInitialized; }

    // used for setting a specific uniform, shouldn't be used except for uniforms which are constant
    template <typename T>
    static bool setUniform(const GLProgram& program, const std::basic_string<GLchar>& name, GLUniformType type, const T& data, size_t count = 1, GLboolean transpose = GL_FALSE)
    {
        GLUniform uniform;
        if ( !uniform.init(program, name, type) )
            return false;
        uniform.loadData(data, count, transpose);
        uniform.set();
        return true;
    }

protected:
    GLUniformType             m_type;
    GLProgram                 m_program;
    GLint                     m_uniform;
    GLboolean                 m_transpose;
    GLint                     m_count;
    int                       m_size;
    bool                      m_isInitialized;

    std::basic_string<GLchar> m_name;
    char*                     m_data;
};

#endif // GLUNIFORM_HPP

