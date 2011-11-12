#include <QLabel>
#include <QImage>

#include "FlatCamera.h"
#include "RayMarcher.h"

#include "MainWindow.h"

static const int imgWidth = 512;
static const int imgHeight = 512;

float scene(const float3 &p)
{
  float plane = dot(float3(0, 0, 1), p);

  float s1 = length(p) - 1;
  float s2 = length(p - float3(0, 0, 1)) - 0.1;

  return qMin(plane, qMin(s1, s2));
}

QImage render()
{
  FlatCamera cam(0.7853981633974483f, float(imgWidth)/float(imgHeight),
                float3(5, 0, 5), float3(0, 0, 0), float3(0, 0, 1));

  QImage result(imgWidth, imgHeight, QImage::Format_ARGB32);
#pragma omp parallel for
  for (int y=0; y<result.height(); y++)
    for (int x=0; x<result.width(); x++)
    {
      HitAccum ha;
      float3 ray = cam.project(x/float(imgWidth-1), 1-y/float(imgHeight-1));

      float z = 0;
      float s = 0;
      float ao = 0;
      float n = 0;
      if (rayMarchHit(cam.pos(), ray, scene, ha))
      {
        z = 1 - qBound(0.0f, ha.z()-3, 10.0f)/10.0f;
        s = ha.steps();
        ao = ambientOcclusion(cam.pos() + ha.z()*ray, ha.normal(), scene);
        n = dot(ha.normal(), ray);
      }

      result.setPixel(x, y, qRgb(255*ao*z + s, 255*ao*z + abs(n*10), 255*ao*z));
     }
  return result;
}

void test()
{
  FlatCamera cam(0.7853981633974483f, float(imgWidth)/float(imgHeight),
                float3(5, 0, 0), float3(0, 0, 0), float3(0, 0, 1));

  HitAccum ha;

  qDebug() << "(0, 0) ->" << cam.project(0, 0);
  qDebug() << "(0, 1) ->" << cam.project(0, 1);
  qDebug() << "(1, 0) ->" << cam.project(1, 0);
  qDebug() << "(1, 1) ->" << cam.project(1, 1);
  qDebug() << "(0.5, 0.5) ->" << cam.project(0.5, 0.5);

  rayMarchHit(cam.pos(), normalize(float3(-1, -1, -1)), scene, ha);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  QLabel *lbl = new QLabel(this);
  lbl->setFixedSize(imgHeight, imgWidth);

  lbl->setPixmap(QPixmap::fromImage(::render()));
  //test();

  setCentralWidget(lbl);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

MainWindow::~MainWindow()
{

}
