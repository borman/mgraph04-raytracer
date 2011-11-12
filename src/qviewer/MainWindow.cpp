#include <QLabel>
#include <QImage>

#include "FlatCamera.h"
#include "Renderer.h"

#include "MainWindow.h"

static const int imgWidth = 512;
static const int imgHeight = 512;

float scene(float3 p)
{
  float plane = dot(float3(0, 0, 1), p);

  float s1 = length(p) - 1;
  float s2 = length(p - float3(0, 0, 1.1)) - 0.1;

  return qMin(plane, qMin(s1, s2));
}

QImage render()
{
  Matrix<Renderer::Pixel> pixbuf(imgWidth, imgHeight);

  FlatCamera cam(M_PI/6, float(imgWidth)/float(imgHeight),
                float3(5, 0, 5), float3(0, 0, 0), float3(0, 0, 1));

  Renderer::render(pixbuf, scene, cam);

  QImage result(imgWidth, imgHeight, QImage::Format_ARGB32);
#pragma omp parallel for
  for (int y=0; y<result.height(); y++)
    for (int x=0; x<result.width(); x++)
    {
      const Renderer::Pixel &pix = pixbuf.at(x, y);
      float3 col = pix.ambientOcclusion * pix.color;
      result.setPixel(x, y, qRgb(255*col.x, 255*col.y, 255*col.z));
    }
  return result;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  QLabel *lbl = new QLabel(this);
  lbl->setFixedSize(imgHeight, imgWidth);

  lbl->setPixmap(QPixmap::fromImage(::render()));

  setCentralWidget(lbl);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

MainWindow::~MainWindow()
{

}
