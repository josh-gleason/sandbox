#ifndef GLCOMPLETEPROGRAM_HPP
#define GLCOMPLETEPROGRAM_HPP

#include "GLProgram.hpp"

class GLCompleteProgram : public GLProgram
{
public:
    GLCompleteProgram();

    bool loadAndLink(const char* vshader, const char* fshader, const char* gshader = nullptr);
};

#endif // GLCOMPLETEPROGRAM_HPP

