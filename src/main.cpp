#include <GL/glew.h>

#include <QApplication>
#include "GLContext.hpp"

#include <iostream>
#include <IL/il.h>

int main(int argc, char *argv[])
{
    if ( argc < 2 )
    {
        std::cerr << "Usage : " << argv[0] << " <modelPath>" << std::endl;
        return -1;
    }

    ilInit();

    QApplication app(argc, argv);

    GLContext context(argv[1]);
    context.show();

    // check if initialization succeeded
    if ( context.good() )
        return app.exec();
    else
        return -1;
}

