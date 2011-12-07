#include <cmath>
#include <algorithm>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "SceneWidget.h"

using std::min;
using std::max;

static const int imgWidth = 512;
static const int imgHeight = 512;

SceneWidget::SceneWidget(Renderer::Scene *scene, QWidget *parent)
  : QWidget(parent),
    m_scene(scene),
    m_image(imgWidth, imgHeight, QImage::Format_ARGB32),
    m_aux(imgWidth, imgHeight, QImage::Format_ARGB32),
    m_needRender(true)
{
  setPhase(0);
}

void SceneWidget::paintEvent(QPaintEvent *)
{
  if (m_needRender)
  {
    render();
    m_needRender = false;
  }
  QPainter p(this);
  p.drawImage(0, 0, m_image);
  //p.drawImage(imgWidth+1, 0, m_aux);
}

static inline QRgb vec2rgb(float3r f)
{
  return qRgb(255*f.x(), 255*f.y(), 255*f.z());
}

static inline QRgb blend(const Renderer::Pixel &p)
{
  return vec2rgb(vbound(float3(0.0f), p.blend(), float3(1.0f)));
}

static QRgb blend_aux(const Renderer::Pixel &p)
{
#if 0
  static const float maxVal = log10f(10000);
  float hblend = qBound(0.0f, log10f(p.steps), maxVal)/maxVal;
#else
  static const float maxVal = 1000;
  float hblend = qBound(float(0.0), float(p.steps), maxVal)/maxVal;
#endif
  return qRgb(0, hblend*255, 0);
  //return vec2rgb(float3(p.reflect));

  //return vec2rgb(vabs(p.normal));
  //return vec2rgb(p.diffuse);

  /*
  float z_norm = qBound(0.0f, log10f(p.z), 2.0f)/2.0f;
  return qRgb(255*z_norm, 255*z_norm, 255*z_norm);
  */

  //return vec2rgb(float3(p.ambientOcclusion));
}

void SceneWidget::render()
{
  QTime time;
  time.start();

  Renderer::Pixel pix;
  float w = imgWidth;
  float h = imgHeight;
#pragma omp parallel for private(pix)
  for (int y=0; y<imgHeight; y++)
    for (int x=0; x<imgWidth; x++)
    {
      m_scene->renderPixel(pix, x/w, 1.0f-y/h);

      ((QRgb *)m_image.scanLine(y))[x] = blend(pix);
      //((QRgb *)m_aux.scanLine(y))[x] = blend_aux(pix);
    }

  qDebug() << "Rendering:" << time.elapsed() << "msec";
}

QSize SceneWidget::sizeHint() const
{
  //return QSize(imgWidth*2+1, imgHeight);
  return QSize(imgWidth, imgHeight);
}

void SceneWidget::mouseDoubleClickEvent(QMouseEvent *)
{
  QImage img(imgWidth*4, imgHeight*4, QImage::Format_ARGB32);

  QTime time;
  time.start();

  Renderer::Pixel pix;
  float w = img.width();
  float h = img.height();
#pragma omp parallel for private(pix)
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      m_scene->renderPixel(pix, x/w, 1.0f-y/h);

      ((QRgb *)img.scanLine(y))[x] = blend(pix);
    }

  qDebug() << "Big rendering:" << time.elapsed() << "msec";
  //img.save("render.png");
  m_image = img.scaled(imgWidth, imgHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  //m_image.save("render_small.png");
  update();
}

void SceneWidget::setPhase(int _phase)
{
  float phase = (_phase * 2 * M_PI)/1000;
  float camr = 3;
  float camx = cos(phase) * camr;
  float camy = sin(phase) * camr;
  float camz = 3.1 + 1*sin(phase);
  m_scene->cam =  FlatCamera(float(M_PI/4.0), float(imgWidth)/float(imgHeight),
                             float3(camx, camy, camz),
                             float3(0.0f, 0.0f, 1.5f),
                             float3(0.0f, 0.0f, 1.0f));
  m_needRender = true;
  update();
}
