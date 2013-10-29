#include <GL/glew.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include "qt/MainApp.hpp"

#include <iostream>
#include <IL/il.h>

int main(int argc, char *argv[])
{
    // initialize devil
    ilInit();

    QApplication app(argc, argv);
    MainApp mainApp;
   
    // determine which screen the mouse is on
    QPoint mousePos = QCursor::pos();
    QDesktopWidget *desk = QApplication::desktop();
    int screenCount = desk->screenCount();
    QRect screenGeometry = desk->screenGeometry(0);
    for ( int i = 0; i < screenCount; ++i )
        if ( desk->screenGeometry(i).contains(mousePos) )
            screenGeometry = desk->screenGeometry(i);

    // center window on current screen (screen where mouse is)
    mainApp.move(screenGeometry.x() + screenGeometry.width()*0.1, screenGeometry.y() + screenGeometry.height()*0.05);
    mainApp.resize(screenGeometry.width()*0.80, screenGeometry.height()*0.90);
    mainApp.show();

    // check if initialization succeeded
    if ( mainApp.good() )
        return app.exec();
    else
        return -1;
}

