#pragma once

#include <gdstk.h>

#include <QMap>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPair>
#include <QTime>
#include <QVector>

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

  struct CellPolygonVertex {
   public:
    CellPolygonVertex();
    ~CellPolygonVertex() = default;
    void clear();
    QVector<QPair<QOpenGLVertexArrayObject *, size_t>> polygon_vaos;
    QOpenGLBuffer *vertex_vbo;
    QOpenGLBuffer *color_vbo;
  };

  QMap<QString, CellPolygonVertex> m_cellname_vertex;
  QString m_current_cellname;
  QOpenGLShaderProgram *m_shader_prog;

  QMatrix4x4 m_proj_matrix;
  QMatrix4x4 m_view_matrix;
  float m_prev_x;
  float m_prev_y;
  float m_scale{1.0};
  bool m_mouse_bt_hold{false};
  bool m_ctrl_pressed{false};

  // grid data
  // QOpenGLBuffer vbo;
  // QOpenGLVertexArrayObject vao1;

  // calculate FPS
  int m_frames{0};
  QTime m_last_time;
};
