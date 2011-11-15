#include <QApplication>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "SceneWidget.h"
#include "MyScene.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    MyScene scene;

    SceneWidget *sceneView = new SceneWidget(&scene, &window);
    QSlider *slider = new QSlider(Qt::Horizontal, &window);
    slider->setRange(0, 1000);
    QObject::connect(slider, SIGNAL(valueChanged(int)), sceneView, SLOT(setPhase(int)));
    QSpinBox *spinbox = new QSpinBox(&window);
    spinbox->setRange(0, 1000);
    QObject::connect(spinbox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), spinbox, SLOT(setValue(int)));

    QVBoxLayout *l = new QVBoxLayout;
    QHBoxLayout *l2 = new QHBoxLayout;
    l->addWidget(sceneView);
    l2->addWidget(slider);
    l2->addWidget(spinbox);
    l->addLayout(l2);
    window.setLayout(l);

    window.show();

    return app.exec();
}
