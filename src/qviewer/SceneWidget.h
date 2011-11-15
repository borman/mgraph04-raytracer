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
  void setPhase(int phase) { m_phase = phase; update(); }
protected:
  void paintEvent(QPaintEvent *);
private:
  void render(float phase);

  Renderer::Scene *m_scene;
  float m_phase;
  QImage m_image;
  QImage m_aux;
};

#endif // SCENEWIDGET_H
