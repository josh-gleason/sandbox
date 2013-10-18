#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>

const int V_POSITION = 0;
const int V_UVCOORD = 1;

// attribute values
const GLfloat vUvCoords[] =
    { 0.0,0.0,  0.0,1.0,  1.0,0.0 };
const GLfloat vPositions[] =
    { -1.0,-1.0,0.0,  -1.0,1.0,0.0,  1.0,-1.0,0.0 };

// 4x4 RGBA texture (black, red, green, blue)
const GLubyte texRGBA[] =
    {0,0,0,255,  255,0,0,255,  0,255,0,0,255,  0,0,255,255,
     0,0,0,255,  255,0,0,255,  0,255,0,0,255,  0,0,255,255,
     0,0,0,255,  255,0,0,255,  0,255,0,0,255,  0,0,255,255,
     0,0,0,255,  255,0,0,255,  0,255,0,0,255,  0,0,255,255};

// globals (just for this example)
GLuint vao;
GLuint texture;
GLuint program;

std::string readSource(const char* path)
{
    // read file (for loading shaders)
    std::ifstream fin(path, std::ios::binary);
    fin.seekg(0,std::ios::end);
    std::string contents(1+fin.tellg(),'\0');
    fin.seekg(0,std::ios::beg);
    fin.read(&contents[0],contents.size()-1);
    fin.close();
    return contents;
}

void initGL()
{
    glewInit();

    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // compile, and attach shaders and link program
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string vsource = readSource("vshader.glsl");
    const GLchar* vsourceC = vsource.c_str();
    std::string fsource = readSource("fshader.glsl");
    const GLchar* fsourceC = fsource.c_str();
    glShaderSource(vshader,1,&vsourceC,0);
    glShaderSource(fshader,1,&fsourceC,0);
    glCompileShader(vshader);
    glCompileShader(fshader);
    program = glCreateProgram();
    glAttachShader(program,vshader);
    glAttachShader(program,fshader);
    glLinkProgram(program);

    // set sampler to sample from GL_TEXTURE0
    GLint sampler = glGetUniformLocation(program, "texMap");
    glProgramUniform1i(program,sampler,0);
    
    // load texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D,
            0,                // level of detail
            GL_RGBA,          // internal format
            4, 4, 0,          // width, height, border(always 0)
            GL_RGBA,          // format
            GL_UNSIGNED_BYTE, // type
            (const GLvoid*)texRGBA);
    glBindTexture(GL_TEXTURE_2D, 0);

    // load buffers
    GLuint buffers[2];
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*9, (const GLvoid*)vPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6, (const GLvoid*)vUvCoords, GL_STATIC_DRAW);
   
    // set attributes
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
        glEnableVertexAttribArray(V_POSITION);
        glEnableVertexAttribArray(V_UVCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glVertexAttribPointer(V_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glVertexAttribPointer(V_UVCOORD, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glBindVertexArray(0);
}

void paintGL()
{
    glClearColor(0.5,0.5,0.5,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,3);
    glBindVertexArray(0);
    glUseProgram(0);
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(300, 200);
    glutCreateWindow("Hello World!");
    initGL();
    glutDisplayFunc(paintGL);
    glutMainLoop();
    return 0;
}
