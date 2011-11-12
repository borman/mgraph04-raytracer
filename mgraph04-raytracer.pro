QT       += core gui

TARGET = mgraph04-raytracer
TEMPLATE = app

VPATH += src/util src/engine src/qviewer
INCLUDEPATH += src/util src/engine

SOURCES += main.cpp\
           MainWindow.cpp \
           FlatCamera.cpp

HEADERS += v3_gen.h \
           float3.h \
           MainWindow.h \
           FlatCamera.h \
           RayMarcher.h \
           HitAccum.h

QMAKE_CXXFLAGS += /openmp /fp:fast /Oi /GS- /GL
QMAKE_CXXFLAGS += /favor:EM64T /Gr

QMAKE_LFLAGS += /LTCG





