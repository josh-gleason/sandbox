#include <GL/glew.h>

#include <QApplication>
#include "GLContext.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GLContext context;
    context.show();

    // check if initialization succeeded
    if ( context.good() )
        return app.exec();
    else
        return -1;
}

