QT       += core gui

TARGET = mgraph04-raytracer
TEMPLATE = app

VPATH += src/util src/engine src/qviewer
INCLUDEPATH += src/util src/engine

SOURCES += main.cpp\
           FlatCamera.cpp \
           Renderer.cpp \
           SceneWidget.cpp

HEADERS += v3_gen.h \
           float3.h \
           float3_sse.h \
           matrix.h \
           FlatCamera.h \
           Renderer.h \
           SceneWidget.h

DEFINES += _USE_MATH_DEFINES
QMAKE_CXXFLAGS += /openmp /fp:fast /Ox /Ot /Ob1 /Oi /GS- /GL
QMAKE_CXXFLAGS += /favor:EM64T /Gr

QMAKE_LFLAGS += /LTCG


