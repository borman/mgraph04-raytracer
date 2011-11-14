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

static inline float3 box(float3 p, float3 b)
{
  return length(vmax(vabs(p) - b, float3(0.0f)));
}

static inline float3 sphere(float3 p, float r)
{
  return length(p) - float3(r);
}

static inline float3 mySphereCuboid(float3 p)
{
  static const float3 bs(0.7f, 0.7f, 0.9f);
  static const float3 c1(-2.0f, -2.0f, 1.3f);
  static const float3 c2(0.0f, 0.0f, 0.8f);

  float3 sd = sphere(p-c1, 1);
  float3 bd = box(p-c2, bs);

  return vmin(sd, bd);
}

static float sceneDist(float3 p)
{
  static const float3 n(0.0f, 0.0f, 1.0f);
  static const float3 bs(0.7f, 0.7f, 0.9f);
  static const float3 c1(0.5f, 0.5f, 1.3f);
  static const float3 c2(0.0f, 0.0f, 1.5f);

  float3 sd = sphere(p-c1, 1);
  float3 bd = box(p-c2, bs);

  return vmin(dot(p, n), vmin(sd, bd)).scalar();
}

static int sceneMat(float3 p)
{
  if (p.z() > 1e-3)
    return 1;
  else
    return 0;
}

SceneWidget::SceneWidget(QWidget *parent)
  : QWidget(parent),
    m_image(imgWidth, imgHeight, QImage::Format_ARGB32),
    m_aux(imgWidth, imgHeight, QImage::Format_ARGB32)
{
  m_scene.dist = sceneDist;
  m_scene.mats = sceneMat;

  Renderer::Material mat;
  mat.ambient = float3(0.3f, 0.3f, 0.3f);
  mat.diffuse = float3(0.5f, 0.5f, 0.5);
  mat.specular = float3(1.0f, 1.0f, 1.0f);
  mat.shininess = 1000;
  m_scene.materials.push_back(mat);

  mat.specular = float3(0.4f, 0.7f,0.4f);
  mat.diffuse = float3(0.3f, 0.4f, 0.2f);
  mat.ambient = 0.5*mat.diffuse;
  m_scene.materials.push_back(mat);

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
  p.drawImage(imgWidth+1, 0, m_aux);
}

static inline QRgb vec2rgb(float3 f)
{
  return qRgb(255*f.x(), 255*f.y(), 255*f.z());
}

static inline QRgb blend(const Renderer::Pixel &p)
{
  return vec2rgb(vbound(float3(0.0f), p.blend(), float3(1.0f)));
}

static QRgb blend_aux(const Renderer::Pixel &p)
{
#if 1
  static const float maxVal = log10f(10000);
  float hblend = qBound(0.0f, log10f(p.steps), maxVal)/maxVal;
#else
  static const float maxVal = 1000;
  float hblend = qBound(0.0f, float(p.steps), maxVal)/maxVal;
#endif
  return qRgb(0, hblend*255, 0);
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
                            float3(0.0f, 0.0f, 0.8f),
                            float3(0.0f, 0.0f, 1.0f));

  QTime time;
  time.start();

  Renderer::Pixel pix;
#pragma omp parallel for private(pix)
  for (int y=0; y<imgHeight; y++)
    for (int x=0; x<imgWidth; x++)
    {
      m_scene.renderPixel(pix, x/w, 1.0f-y/h);

      ((QRgb *)m_image.scanLine(y))[x] = blend(pix);
      ((QRgb *)m_aux.scanLine(y))[x] = blend_aux(pix);
    }

  qDebug() << "Rendering:" << time.elapsed() << "msec";
}

QSize SceneWidget::sizeHint() const
{
  return QSize(imgWidth*2+1, imgHeight);
}
