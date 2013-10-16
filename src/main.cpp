#include <GL/glew.h>

#include <QApplication>
#include "qt/MainApp.hpp"

#include <iostream>
#include <IL/il.h>

int main(int argc, char *argv[])
{
    if ( argc < 2 )
    {
        std::cerr << "Usage : " << argv[0] << " <modelPath> [option]" << std::endl;
        return -1;
    }

    bool flipUvs = false;
    if ( argc >= 3 )
        flipUvs = std::string(argv[2]) == "flipuvs";

    // initialize devil
    ilInit();

    QApplication app(argc, argv);

    MainApp mainApp(argv[1], flipUvs);
    mainApp.show();

    // check if initialization succeeded
    if ( mainApp.good() )
        return app.exec();
    else
        return -1;
}

