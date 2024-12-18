#pragma once

#include <gdstk.h>

#include <QHash>
#include <QPair>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QVector>

class LayoutCanvas : public QOpenGLWidget {
  Q_OBJECT

 public:
  LayoutCanvas(QWidget *parent = nullptr);
  ~LayoutCanvas();

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;
  bool event(QEvent *e) override;

  void wheelEvent(QWheelEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

 private:
  void parseGds(const gdstk::Library& lib);
  void makePolygons(gdstk::Polygon *);
  void clear();

  QOpenGLFunctions *m_gl_func;
  QVector<QOpenGLBuffer *> m_border_vbos;
  QHash<QOpenGLVertexArrayObject *, QPair<quint64, QVector3D>> m_border_vaos;
  QVector<QOpenGLBuffer *> m_fill_vbos;
  QHash<QOpenGLVertexArrayObject *, QPair<quint64, QVector3D>> m_fill_vaos;
  QOpenGLShaderProgram *m_border_shader;
  QOpenGLShaderProgram *m_fill_shader;

  QMatrix4x4 m_view_matrix;
  QMatrix4x4 m_model_matrix;
  QMatrix4x4 m_proj_matrix;
  float m_pan_x{0};
  float m_pan_y{0};
  float m_prev_x_for_pan;
  float m_prev_y_for_pan;
  uint m_color;
  uint m_modelToWorld;
  uint m_worldToCamera;
  uint m_cameraToView;
  bool m_panning{false};
  bool m_ctrl_pressed{false};
};
