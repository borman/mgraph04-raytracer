QT       += core gui

TARGET = mgraph04-raytracer
TEMPLATE = app

VPATH += src/util src/engine src/qviewer
INCLUDEPATH += src/util src/engine

SOURCES += main.cpp\
           FlatCamera.cpp \
           Renderer.cpp \
           SceneWidget.cpp \
           MyScene.cpp

HEADERS += v3_gen.h \
           float3.h \
           float3_sse.h \
           matrix.h \
           FlatCamera.h \
           Renderer.h \
           SceneWidget.h \
           MyScene.h

win32 {
  DEFINES += _USE_MATH_DEFINES
  QMAKE_CXXFLAGS += /openmp /fp:fast /GL
  !debug {
    QMAKE_CXXFLAGS += /Ox /Ot /Ob1 /Oi /GS-
  }

  QMAKE_LFLAGS += /arch:AVX /LTCG
}

unix {
  QMAKE_CXXFLAGS += -msse4 -fopenmp -fno-exceptions -fno-rtti
  !debug {
    QMAKE_CXXFLAGS += -flto -Ofast
  }

  QMAKE_LFLAGS += -fopenmp
}




