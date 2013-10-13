#include "GLUniform.hpp"
    
GLUniform::GLUniform() :
    m_type(),
    m_program(),
    m_uniform(-1),
    m_transpose(false),
    m_count(0),
    m_size(1),
    m_isInitialized(false),
    m_name(""),
    m_data(nullptr)
{}

GLUniform::GLUniform(GLProgram program, const std::basic_string<GLchar>& name, GLUniformType type) : GLUniform()
{
    this->init(program, name, type);
}

GLUniform::GLUniform(const GLUniform &rhs) :
    m_type(rhs.m_type),
    m_program(rhs.m_program),
    m_uniform(rhs.m_uniform),
    m_transpose(rhs.m_transpose),
    m_count(rhs.m_count),
    m_size(rhs.m_size),
    m_isInitialized(rhs.m_isInitialized),
    m_name(rhs.m_name)
{
    if ( rhs.m_data == nullptr )
    {
        m_data = rhs.m_data;
    }
    else
    {
        m_data = new char[m_count*m_size];
        for ( int i = 0; i < m_count*m_size; ++i )
            m_data[i] = rhs.m_data[i];
    }
}

GLUniform::~GLUniform()
{
    if ( m_data != nullptr )
        delete [] m_data;
}

GLUniform& GLUniform::operator=(const GLUniform& rhs)
{
    m_type = rhs.m_type;
    m_program = rhs.m_program;
    m_uniform = rhs.m_uniform;
    m_transpose = rhs.m_transpose;
    m_count = rhs.m_count;
    m_size = rhs.m_size;
    m_isInitialized = rhs.m_isInitialized;
    m_name = rhs.m_name;

    if ( m_data != nullptr )
        delete [] m_data;

    if ( rhs.m_data == nullptr )
    {
        m_data = rhs.m_data;
    }
    else
    {
        m_data = new char[m_count*m_size];
        for ( int i = 0; i < m_count*m_size; ++i )
            m_data[i] = rhs.m_data[i];
    }

    return *this;
}

bool GLUniform::init(GLProgram program, const std::basic_string<GLchar>& name, GLUniformType type)
{
    m_uniform = glGetUniformLocation(program.getProgramIdx(), name.c_str());
    if ( m_uniform == -1 )
        return false;
    m_program = program;
    m_type = type;
    m_isInitialized = true;
    return true;
}

void GLUniform::set()
{
    switch (m_type)
    {
        case INT_V:
        {
            glProgramUniform1iv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLint*>(m_data));
            break;
        }
        case UINT_V:
        {
            glProgramUniform1uiv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLuint*>(m_data));
            break;
        }
        case FLOAT_V:
        {
            glProgramUniform1fv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case DOUBLE_V:
        {
            glProgramUniform1dv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case VEC2UI_V:
        {
            glProgramUniform2uiv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLuint*>(m_data));
            break;
        }
        case VEC3UI_V:
        {
            glProgramUniform3uiv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLuint*>(m_data));
            break;
        }
        case VEC4UI_V:
        {
            glProgramUniform4uiv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLuint*>(m_data));
            break;
        }
        case VEC2I_V:
        {
            glProgramUniform2iv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLint*>(m_data));
            break;
        }
        case VEC3I_V:
        {
            glProgramUniform3iv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLint*>(m_data));
            break;
        }
        case VEC4I_V:
        {
            glProgramUniform4iv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLint*>(m_data));
            break;
        }
        case VEC2F_V:
        {
            glProgramUniform2fv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case VEC3F_V:
        {
            glProgramUniform3fv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case VEC4F_V:
        {
            glProgramUniform4fv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case VEC2D_V:
        {
            glProgramUniform2dv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case VEC3D_V:
        {
            glProgramUniform3dv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case VEC4D_V:
        {
            glProgramUniform4dv(m_program.getProgramIdx(), m_uniform, m_count, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case INT:
        {
            glProgramUniform1i(m_program.getProgramIdx(), m_uniform, *reinterpret_cast<const GLint*>(m_data));
            break;
        }
        case UINT:
        {
            glProgramUniform1ui(m_program.getProgramIdx(), m_uniform, *reinterpret_cast<const GLint*>(m_data));
            break;
        }
        case FLOAT:
        {
            glProgramUniform1f(m_program.getProgramIdx(), m_uniform, *reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case DOUBLE:
        {
            glProgramUniform1d(m_program.getProgramIdx(), m_uniform, *reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case VEC2UI:
        {
            glProgramUniform2ui(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLuint*>(m_data),
                    *reinterpret_cast<const GLuint*>(m_data+sizeof(GLuint)));
            break;
        }
        case VEC3UI:
        {
            glProgramUniform3ui(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLuint*>(m_data),
                    *reinterpret_cast<const GLuint*>(m_data+sizeof(GLuint)),
                    *reinterpret_cast<const GLuint*>(m_data+2*sizeof(GLuint)));
            break;
        }
        case VEC4UI:
        {
            glProgramUniform4ui(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLuint*>(m_data),
                    *reinterpret_cast<const GLuint*>(m_data+sizeof(GLuint)),
                    *reinterpret_cast<const GLuint*>(m_data+2*sizeof(GLuint)),
                    *reinterpret_cast<const GLuint*>(m_data+3*sizeof(GLuint)));
            break;
        }
        case VEC2I:
        {
            glProgramUniform2i(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLint*>(m_data),
                    *reinterpret_cast<const GLint*>(m_data+sizeof(GLint)));
            break;
        }
        case VEC3I:
        {
            glProgramUniform3i(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLint*>(m_data),
                    *reinterpret_cast<const GLint*>(m_data+sizeof(GLint)),
                    *reinterpret_cast<const GLint*>(m_data+2*sizeof(GLint)));
            break;
        }
        case VEC4I:
        {
            glProgramUniform4i(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLint*>(m_data),
                    *reinterpret_cast<const GLint*>(m_data+sizeof(GLint)),
                    *reinterpret_cast<const GLint*>(m_data+2*sizeof(GLint)),
                    *reinterpret_cast<const GLint*>(m_data+3*sizeof(GLint)));
            break;
        }
        case VEC2F:
        {
            glProgramUniform2f(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLfloat*>(m_data),
                    *reinterpret_cast<const GLfloat*>(m_data+sizeof(GLfloat)));
            break;
        }
        case VEC3F:
        {
            glProgramUniform3f(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLfloat*>(m_data),
                    *reinterpret_cast<const GLfloat*>(m_data+sizeof(GLfloat)),
                    *reinterpret_cast<const GLfloat*>(m_data+2*sizeof(GLfloat)));
            break;
        }
        case VEC4F:
        {
            glProgramUniform4f(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLfloat*>(m_data),
                    *reinterpret_cast<const GLfloat*>(m_data+sizeof(GLfloat)),
                    *reinterpret_cast<const GLfloat*>(m_data+2*sizeof(GLfloat)),
                    *reinterpret_cast<const GLfloat*>(m_data+3*sizeof(GLfloat)));
            break;
        }
        case VEC2D:
        {
            glProgramUniform2d(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLdouble*>(m_data),
                    *reinterpret_cast<const GLdouble*>(m_data+sizeof(GLdouble)));
            break;
        }
        case VEC3D:
        {
            glProgramUniform3d(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLdouble*>(m_data),
                    *reinterpret_cast<const GLdouble*>(m_data+sizeof(GLdouble)),
                    *reinterpret_cast<const GLdouble*>(m_data+2*sizeof(GLdouble)));
            break;
        }
        case VEC4D:
        {
            glProgramUniform4d(m_program.getProgramIdx(), m_uniform,
                    *reinterpret_cast<const GLdouble*>(m_data),
                    *reinterpret_cast<const GLdouble*>(m_data+sizeof(GLdouble)),
                    *reinterpret_cast<const GLdouble*>(m_data+2*sizeof(GLdouble)),
                    *reinterpret_cast<const GLdouble*>(m_data+3*sizeof(GLdouble)));
            break;
        }
        case MAT2F:
        {
            glProgramUniformMatrix2fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT3F:
        {
            glProgramUniformMatrix3fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT4F:
        {
            glProgramUniformMatrix4fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT2X3F:
        {
            glProgramUniformMatrix2x3fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT3X2F:
        {
            glProgramUniformMatrix3x2fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT2X4F:
        {
            glProgramUniformMatrix2x4fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT4X2F:
        {
            glProgramUniformMatrix4x2fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT3X4F:
        {
            glProgramUniformMatrix3x4fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT4X3F:
        {
            glProgramUniformMatrix4x3fv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLfloat*>(m_data));
            break;
        }
        case MAT2D:
        {
            glProgramUniformMatrix2dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT3D:
        {
            glProgramUniformMatrix3dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT4D:
        {
            glProgramUniformMatrix4dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT2X3D:
        {
            glProgramUniformMatrix2x3dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT3X2D:
        {
            glProgramUniformMatrix3x2dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT2X4D:
        {
            glProgramUniformMatrix2x4dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT4X2D:
        {
            glProgramUniformMatrix4x2dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
        case MAT3X4D:
        {
            glProgramUniformMatrix3x4dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;    
        }
        case MAT4X3D:
        {
            glProgramUniformMatrix4x3dv(m_program.getProgramIdx(), m_uniform, m_count, m_transpose, reinterpret_cast<const GLdouble*>(m_data));
            break;
        }
    }
}


