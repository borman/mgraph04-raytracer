#include <QApplication>
#include <QSlider>
#include <QVBoxLayout>
#include "SceneWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;

    SceneWidget *scene = new SceneWidget(&window);
    QSlider *slider = new QSlider(Qt::Horizontal, &window);
    slider->setRange(0, 1000);
    QObject::connect(slider, SIGNAL(valueChanged(int)), scene, SLOT(setPhase(int)));

    QVBoxLayout *l = new QVBoxLayout(&window);
    l->addWidget(scene);
    l->addWidget(slider);
    window.setLayout(l);

    window.show();

    return app.exec();
}
