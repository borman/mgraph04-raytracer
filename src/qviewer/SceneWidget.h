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
  explicit SceneWidget(Renderer::Scene *scene, QWidget *parent = 0);
  QSize sizeHint() const;
public slots:
  void setPhase(int phase);
protected:
  void paintEvent(QPaintEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);
private:
  void render();

  Renderer::Scene *m_scene;
  QImage m_image;
  QImage m_aux;
  bool m_needRender;
};

#endif // SCENEWIDGET_H
