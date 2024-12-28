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
  void test();
  void initializeGrid();

  QMap<QString, std::vector<CellPolygonVertex>> m_cellname_vertex;
  QString m_current_cellname;
  QOpenGLShaderProgram *m_shader;

  QMatrix4x4 m_view_matrix;
  QMatrix4x4 m_model_matrix;
  QMatrix4x4 m_proj_matrix;
  float m_pan_x{0};
  float m_pan_y{0};
  float m_prev_x_for_pan;
  float m_prev_y_for_pan;
  bool m_mouse_bt_hold{false};
  bool m_ctrl_pressed{false};

  QOpenGLBuffer vbo;
  QOpenGLVertexArrayObject vao1;
};
