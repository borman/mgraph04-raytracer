#include <QApplication>
#include "SceneWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SceneWidget w;
    w.show();

    return a.exec();
}
