#include <algorithm>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "SceneWidget.h"

using std::min;
using std::max;

static const int imgWidth = 512;
static const int imgHeight = 512;

static inline float3 box(float3 p, float3 b)
{
  return length(vmax(vabs(p) - b, float3(0.0f)));
}

static inline float3 sphere(float3 p, float r)
{
  return length(p) - float3(r);
}

static float sceneDist(float3 p)
{
  static const float3 bs(0.7f, 0.7f, 0.9f);
  static const float3 c1(0.3f, -0.3f, 1.0f);
  static const float3 c2(0.0f, 0.0f, 0.8f);
  static const float3 n(0.0f, 0.0f, 1.0f);
  return vmin(dot(p, n), vmin(sphere(p-c1, 1), box(p-c2, bs))).scalar();
}

SceneWidget::SceneWidget(QWidget *parent)
  : QWidget(parent),
    m_image(imgWidth, imgHeight, QImage::Format_ARGB32)
{
  m_scene.dist = sceneDist;
  m_scene.ambient = float3(0.3f, 0.3f, 0.3f);
  m_scene.diffuse = float3(0.5f, 0.5f, 0.5);
  m_scene.specular = float3(1.0f, 1.0f, 1.0f);
  m_scene.shininess = 1000;
  m_scene.lights.push_back(
        Renderer::Light(float3(10.0f, 10.0f, 10.0f),
                        float3(0.8f, 0.2f, 0.2f),
                        0.03f, 0.0f, 0.005f));
  m_scene.lights.push_back(
        Renderer::Light(float3(-3.0f, 1.0f, 4.0f),
                        float3(0.1f, 0.8f, 0.1f),
                        0.1f, 0.0f, 0.05f));
  m_scene.lights.push_back(
        Renderer::Light(float3(-1.0f, -5.0f, 2.0f),
                        float3(0.1f, 0.1f, 0.8f),
                        0.3f, 0.0f, 0.01f));
}

void SceneWidget::paintEvent(QPaintEvent *)
{
  render(m_phase*2*M_PI/1000);
  QPainter p(this);
  p.drawImage(0, 0, m_image);
}

static float3 blend(const Renderer::Pixel &p)
{
  static const float3 mask(1.0, 0.0, 0.0);

  float3 color = p.ambientOcclusion * p.ambient
                 + p.diffuse
                 + p.specular;
#if 0
  float hblend = qBound(0, p.steps, 100)/100.0;
  float3 res = (1-hblend) * color + hblend * mask;
#else
  float3 res = color;
#endif
  return vbound(float3(0.0f), res, float3(1.0f));
}

void SceneWidget::render(float phase)
{
  float w = imgWidth;
  float h = imgHeight;

  float camr = 3;
  float camx = cos(phase) * camr;
  float camy = sin(phase) * camr;
  float camz = 3.1 + 3*sin(phase);
  m_scene.cam =  FlatCamera(float(M_PI/4.0), w/h,
                            float3(camx, camy, camz),
                            float3(0.0f, 0.0f, 0.5f),
                            float3(0.0f, 0.0f, 1.0f));

  QTime time;
  time.start();

  Renderer::Pixel pix;
#pragma omp parallel for private(pix)
  for (int y=0; y<imgHeight; y++)
    for (int x=0; x<imgWidth; x++)
    {
      m_scene.renderPixel(pix, x/w, 1.0f-y/h);
      float3 col = blend(pix);

      ((QRgb *)m_image.scanLine(y))[x] = qRgb(255*col.x(), 255*col.y(), 255*col.z());
    }

  qDebug() << "Rendering:" << time.elapsed() << "msec";
}

QSize SceneWidget::sizeHint() const
{
  return QSize(imgWidth, imgHeight);
}
