QT       += core gui

TARGET = mgraph04-raytracer
TEMPLATE = app

VPATH += src/util src/engine src/qviewer
INCLUDEPATH += src/util src/engine

SOURCES += main.cpp\
           FlatCamera.cpp \
           Renderer.cpp \
           SceneWidget.cpp \
    src/qviewer/MyScene.cpp

HEADERS += v3_gen.h \
           float3.h \
           float3_sse.h \
           matrix.h \
           FlatCamera.h \
           Renderer.h \
           SceneWidget.h \
    src/qviewer/MyScene.h

win32 {
  DEFINES += _USE_MATH_DEFINES
  QMAKE_CXXFLAGS += /openmp /fp:fast /GL
  release {
    QMAKE_CXXFLAGS += /Ox /Ot /Ob1 /Oi /GS-
  }

  QMAKE_LFLAGS += /LTCG
}

unix {
  QMAKE_CXXFLAGS += -msse4 -march=corei7 -fopenmp -flto -Ofast -fno-exceptions -fno-rtti
  QMAKE_LFLAGS += -fopenmp
}




