#include <QPainter>
#include <QTimer>
#include "SceneWidget.h"

static const int imgWidth = 512;
static const int imgHeight = 512;

SceneWidget::SceneWidget(QWidget *parent)
  : QWidget(parent),
    m_image(imgWidth, imgHeight, QImage::Format_ARGB32),
    m_pixbuf(imgWidth, imgHeight)
{
  m_timer.start();

  QTimer *updater = new QTimer(this);
  updater->start(1000/25);
  connect(updater, SIGNAL(timeout()), SLOT(update()));
}

void SceneWidget::paintEvent(QPaintEvent *)
{
  render(m_timer.elapsed() / 1000.0);
  QPainter p(this);
  p.drawImage(0, 0, m_image);
}

static float scene(float3 p)
{
  float plane = dot(float3(0, 0, 1), p);

  float s1 = length(p) - 1;
  float s2 = length(p - float3(0, 0.3, 1.0)) - 0.1;

  return qMin(plane, qMin(s1, s2));
}

void SceneWidget::render(float phase)
{
  float camr = 5;
  float camx = cos(phase) * camr;
  float camy = sin(phase) * camr;
  float camz = 5 + 3*sin(phase);
  FlatCamera cam(M_PI/6, float(imgWidth)/float(imgHeight),
                float3(camx, camy, camz), float3(0, 0, 0.5), float3(0, 0, 1));

  QTime time;
  time.start();

  Renderer::render(m_pixbuf, scene, cam);

#pragma omp parallel for
  for (int y=0; y<imgHeight; y++)
    for (int x=0; x<imgWidth; x++)
    {
      const Renderer::Pixel &pix = m_pixbuf.at(x, y);

      float hblend = qBound(0, pix.steps, 100)/100.0;
      float3 col = (1-hblend) * pix.ambientOcclusion * pix.color
                 + hblend     * float3(1.0, 0.0, 0.0);
      ((QRgb *)m_image.scanLine(y))[x] = qRgb(255*col.x, 255*col.y, 255*col.z);
    }

  qDebug() << "Rendering:" << time.elapsed() << "msec";
}

QSize SceneWidget::sizeHint() const
{
  return QSize(imgWidth, imgHeight);
}
