#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include <QTime>
#include <QImage>
#include "Renderer.h"

class SceneWidget : public QWidget
{
  Q_OBJECT
public:
  explicit SceneWidget(QWidget *parent = 0);
  QSize sizeHint() const;
protected:
  void paintEvent(QPaintEvent *);
private:
  void render(float phase);

  QTime m_timer;
  QImage m_image;
  Matrix<Renderer::Pixel> m_pixbuf;
};

#endif // SCENEWIDGET_H
