#pragma once

#include <gdstk.h>

#include <QMap>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPair>
#include <QVector>
#include <QTime>

struct CellPolygonVertex {
 public:
  void reset();
  QOpenGLVertexArrayObject *vao{nullptr};
  QOpenGLBuffer *vertex_buffer{nullptr};
  QOpenGLBuffer *color_buffer{nullptr};
  int vertex_count{0};
};

class LayoutCanvas : public QOpenGLWidget, protected QOpenGLFunctions {
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
  void parseGds(const gdstk::Library &lib);
  void makeCellPolygons(gdstk::Cell *);
  void clear();
  void initializeGrid();
  void calculateFPS();

  QMap<QString, std::vector<CellPolygonVertex>> m_cellname_vertex;
  QString m_current_cellname;
  QOpenGLShaderProgram *m_shader;

  QMatrix4x4 m_proj_matrix;
  QMatrix4x4 m_view_matrix;
  float m_prev_x;
  float m_prev_y;
  float m_scale{1.0};
  bool m_mouse_bt_hold{false};
  bool m_ctrl_pressed{false};

  QOpenGLBuffer vbo;
  QOpenGLVertexArrayObject vao1;

  // used for calculating FPS
  int m_frames{0};
  QTime m_last_time;
};
