######################################################################
# Automatically generated by qmake (3.0) Sun Sep 22 21:59:34 2013
######################################################################

# add opengl functionality
QT += opengl

QMAKE_CC = clang
#QMAKE_CXX = g++
QMAKE_CXX = clang++
QMAKE_CXXFLAGS = -std=c++11 -g
TEMPLATE = app
TARGET = ../bin/main
DEPENDPATH += .

LIBS += -lGLEW -lassimp -lIL -lboost_system -lboost_filesystem

#Optional opencv usage
#INCLUDEPATH += /usr/local/include/opencv /usr/local/include
#LIBS += ../lib/libglimg.a -lboost_system -lboost_filesystem /usr/local/lib/libopencv_calib3d.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libopencv_nonfree.so /usr/local/lib/libopencv_objdetect.so /usr/local/lib/libopencv_photo.so /usr/local/lib/libopencv_stitching.so /usr/local/lib/libopencv_ts.so /usr/local/lib/libopencv_video.so /usr/local/lib/libopencv_videostab.so

CONFIG += warn_on
CONFIG += debug
# copy directory files into temp when compiling not just files
CONFIG += copy_dir_files

# Input
SOURCES += ../src/shapes/*.cpp
SOURCES += ../src/view/*.cpp
SOURCES += ../src/glwrappers/*.cpp
SOURCES += ../src/qt/*.cpp
SOURCES += ../src/*.cpp
HEADERS += ../src/shapes/*.hpp
HEADERS += ../src/view/*.hpp
HEADERS += ../src/glwrappers/*.hpp
HEADERS += ../src/qt/*.hpp
HEADERS += ../src/*.hpp

