#include "LayoutCanvas.h"

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QtGlobal>

#include "Triangulate.h"
#include "event/CloseFileEvent.h"
#include "event/EventDispacher.h"
#include "event/OpenFileEvent.h"
#include "event/SetCellNamesEvent.h"
#include "gdstk.h"

namespace {
const QVector3D DEFAULT_LAYER_COLOR[] = {
    {0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f},
    {0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.0f},
    {0.5f, 0.0f, 0.5f}, {0.0f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.5f},
    {0.0f, 1.0f, 1.0f}, {0.5f, 0.0f, 1.0f}, {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 1.0f}, {0.5f, 1.0f, 0.0f}, {0.5f, 1.0f, 0.5f},
    {0.5f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.5f}, {1.0f, 0.0f, 1.0f},
    {1.0f, 0.5f, 0.0f}, {1.0f, 0.5f, 0.5f}, {1.0f, 0.5f, 1.0f},
    {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f},
};

}  // namespace

void CellPolygonVertex::reset() {
  if (vao) {
    vao->destroy();
    vao = nullptr;
  }
  if (vertex_buffer) {
    vertex_buffer->destroy();
    vertex_buffer = nullptr;
  }
  if (color_buffer) {
    color_buffer->destroy();
    color_buffer = nullptr;
  }
  vertex_count = 0;
}

LayoutCanvas::LayoutCanvas(QWidget* parent) : QOpenGLWidget(parent) {
  EventDispacher::instance().registComp(EventComp::LayoutCanvas, this);
  setFocusPolicy(Qt::WheelFocus);
}

LayoutCanvas::~LayoutCanvas() {
  // Make sure the context is current and then explicitly
  // destroy all underlying OpenGL resources.
  makeCurrent();
  for (auto&& i : m_cellname_vertex) {
    for (auto&& j : i) {
      j.reset();
    }
  }
  m_cellname_vertex.clear();
  if (m_shader) {
    delete m_shader;
    m_shader = nullptr;
  }
  doneCurrent();
}

void LayoutCanvas::initializeGL() {
  initializeOpenGLFunctions();
  // clean background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // set background color
  glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
  // enable alpha blend
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // bind shader
  m_shader = new QOpenGLShaderProgram();
  m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                    ":/shader/vertex.glsl");
  m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                    ":/shader/fragment.glsl");
  if (m_shader->link()) {
    qDebug() << "Shaders link success.";
  } else {
    qDebug() << "Shaders link failed!";
  }

  m_view_matrix.lookAt(QVector3D(0.f, 0.f, -1.f), QVector3D(0.f, 0.f, 0.f),
                       QVector3D(0.f, 1.f, 0.f));
  m_model_matrix.scale(1.f, 1.f);
  // TODO:draw background mesh
  // initializeGrid();
}

void LayoutCanvas::paintGL() {
  // clean background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // set background color
  glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
  m_shader->bind();
  m_shader->setUniformValue("modelToWorld", m_model_matrix);
  m_shader->setUniformValue("worldToCamera", m_view_matrix);
  m_shader->setUniformValue("cameraToView", m_proj_matrix);

  if (m_cellname_vertex.count(m_current_cellname)) {
    const auto& cell_vertex = m_cellname_vertex.value(m_current_cellname);
    for (auto&& i : cell_vertex) {
      i.vao->bind();
      // draw polygon border
      glLineWidth(3.0f);
      glDrawArrays(GL_LINE_LOOP, 0, i.vertex_count);
      i.vao->release();
    }
  }

  m_shader->release();
}

void LayoutCanvas::resizeGL(int w, int h) {
  m_proj_matrix.setToIdentity();
  m_proj_matrix.perspective(45.0f, w / float(h), 0.0f, 1000.0f);
  update();
}

void LayoutCanvas::initializeGrid() {
  // 设置网格范围和密度
  float gridSize = 1.0f;  // 网格的半径范围
  int gridLines = 10;     // 每方向的线条数

  QVector<float> gridVertices;
  for (int i = 0; i <= gridLines; ++i) {
    float pos = -gridSize + (2.0f * gridSize / gridLines) * i;

    // 水平线
    gridVertices << -gridSize << pos << 0.0f;
    gridVertices << gridSize << pos << 0.0f;

    // 垂直线
    gridVertices << pos << -gridSize << 0.0f;
    gridVertices << pos << gridSize << 0.0f;
  }

  // 创建 VBO 和 VAO
  vao1.create();
  vao1.bind();

  vbo.create();
  vbo.bind();
  vbo.allocate(gridVertices.data(), gridVertices.size() * sizeof(float));

  m_shader->enableAttributeArray(0);
  m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));

  vbo.release();
  vao1.release();
}

void LayoutCanvas::parseGds(const gdstk::Library& lib) {
  clear();
  // get top cells, see gdstk documentation for more info
  gdstk::Array<gdstk::Cell*> cells = {};
  gdstk::Array<gdstk::RawCell*> raw_cells = {};
  lib.top_level(cells, raw_cells);

  QVector<QString> cell_names;
  for (size_t i = 0; i < cells.count; i++) {
    auto& cell = cells[i];
    cell_names.push_back(QString::fromStdString(cell->name));
    makeCellPolygons(cell);
  }

  QApplication::postEvent(
      EventDispacher::instance().getComp(EventComp::CellList),
      new SetCellNamesEvent(cell_names));

  if (cells.count) {
    auto& cell = cells[0];
    m_current_cellname = cells[0]->name;

#if 0
    gdstk::Vec2 min = {};
    gdstk::Vec2 max = {};
    cell->bounding_box(min, max);
    double box_width = max.x - min.x;
    double box_height = max.y - min.y;
    int width = this->width();
    int height = this->height();
    double width_ratio = width * 0.8 / box_width;
    double height_ratio = height * 0.8 / box_height;

    double center_x = min.x + box_width / 2;
    double center_y = min.y + box_height / 2;

    float distance = m_view_matrix.column(3).z();
    distance = distance * std::min(width_ratio, height_ratio);
    // m_view_matrix.lookAt(QVector3D(0.f, 0.f, -distance),
    //                      QVector3D(center_x, center_y, 0.f),
    //                      QVector3D(0.f, 1.f, 0.f));
#endif
  }
  update();
}

void LayoutCanvas::makeCellPolygons(gdstk::Cell* cell) {
  makeCurrent();
  // recrusively get all polygons of current cell
  gdstk::Array<gdstk::Polygon*> polygons = {0};
  cell->get_polygons(true, true, -1, false, 0, polygons);

  auto& cell_vertex = m_cellname_vertex[cell->name];

  for (size_t i = 0; i < polygons.count; i++) {
    auto gds_polygon = polygons[i];
    Vector2dVector p;
    QVector<GLfloat> all_vertex;
    for (size_t i = 0; i < gds_polygon->point_array.count; i++) {
      auto& point = gds_polygon->point_array[i];
      all_vertex.push_back(point.x);
      all_vertex.push_back(point.y);
      // p.push_back(Vector2d(point.x, point.y));
    }
    // TODO:triangluate, divide polygon to triangles
    // Vector2dVector all_vertex;
    // Triangulate::Process(p, all_vertex);

    // different layer has different color
    uint32_t layer = gdstk::get_layer(gds_polygon->tag);
    auto color = DEFAULT_LAYER_COLOR[layer % sizeof(DEFAULT_LAYER_COLOR)];

    // color of each vertex
    QVector<GLfloat> vertex_color;
    for (int i = 0; i < all_vertex.size(); i += 2) {
      vertex_color.push_back(color.x());
      vertex_color.push_back(color.y());
      vertex_color.push_back(color.z());
    }

    cell_vertex.emplace_back();
    auto& polygon_vertex = cell_vertex.back();
    polygon_vertex.vao = new QOpenGLVertexArrayObject();
    polygon_vertex.vao->create();
    polygon_vertex.vao->bind();

    polygon_vertex.vertex_buffer =
        new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    polygon_vertex.vertex_buffer->create();
    polygon_vertex.vertex_buffer->bind();
    // insert vertex and color
    polygon_vertex.vertex_buffer->allocate(all_vertex.data(),
                                           all_vertex.size() * sizeof(GLfloat));
    m_shader->bind();
    m_shader->enableAttributeArray(0);
    m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 2);
    m_shader->release();

    polygon_vertex.color_buffer =
        new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    polygon_vertex.color_buffer->create();
    polygon_vertex.color_buffer->bind();
    polygon_vertex.color_buffer->allocate(
        vertex_color.data(), vertex_color.size() * sizeof(GLfloat));

    m_shader->bind();
    m_shader->enableAttributeArray(1);
    m_shader->setAttributeBuffer(1, GL_FLOAT, 0, 3);
    m_shader->release();

    polygon_vertex.vertex_count = all_vertex.size() / 2;
    polygon_vertex.vao->release();
    polygon_vertex.vertex_buffer->release();
    polygon_vertex.color_buffer->release();
  }

  polygons.clear();
  doneCurrent();
}

void LayoutCanvas::clear() {
  makeCurrent();
  for (auto&& i : m_cellname_vertex) {
    for (auto&& j : i) {
      j.reset();
    }
  }
  m_cellname_vertex.clear();
  doneCurrent();
  update();
}

// --------------------- keyboard mouse event ---------------------------------
void LayoutCanvas::mouseMoveEvent(QMouseEvent* event) {
  if (!m_mouse_bt_hold) {
    return;
  }

  float x = event->pos().x();
  float y = event->pos().y();

  float distance = m_view_matrix.column(3).z();

  m_pan_x = (m_pan_x + (x - m_prev_x_for_pan) * qAbs(distance) / 500.f);
  m_pan_y = (m_pan_y + (y - m_prev_y_for_pan) * qAbs(distance) / 500.f);

  m_prev_x_for_pan = x;
  m_prev_y_for_pan = y;

  m_view_matrix.setColumn(3, QVector4D(m_pan_x, -m_pan_y, distance, 1.f));

  update();
}

void LayoutCanvas::mousePressEvent(QMouseEvent* event) {
  switch (event->button()) {
    case Qt::LeftButton: {
      if (m_mouse_bt_hold) {
        return;
      }
      m_mouse_bt_hold = true;
      m_prev_x_for_pan = event->pos().x();
      m_prev_y_for_pan = event->pos().y();
      break;
    }
    default: {
      return QOpenGLWidget::mouseMoveEvent(event);
    }
  }
}

void LayoutCanvas::mouseReleaseEvent(QMouseEvent* event) {
  Q_UNUSED(event);

  if (!m_mouse_bt_hold) {
    return;
  }
  m_mouse_bt_hold = false;
}

bool LayoutCanvas::event(QEvent* e) {
  if (e->type() == OpenFileEvent::type) {
    clear();
    QString fn = dynamic_cast<OpenFileEvent*>(e)->fileName();
    gdstk::ErrorCode error_code = gdstk::ErrorCode::NoError;
    gdstk::Set<gdstk::Tag> tags = {0};
    auto lib =
        gdstk::read_gds(fn.toLocal8Bit().data(), 0, 1e-2, NULL, &error_code);
    if (error_code != gdstk::ErrorCode::NoError) {
      qDebug() << "Error reading GDS file";
      e->ignore();
      return false;
    }
    clear();
    parseGds(lib);
    lib.free_all();
    e->accept();
    return true;
  } else if (e->type() == CloseFileEvent::type) {
    clear();
    e->accept();
    return true;
  } else {
    return QOpenGLWidget::event(e);
  }
}

void LayoutCanvas::wheelEvent(QWheelEvent* event) {
  float scale = 0;
  if (m_ctrl_pressed) {
    scale = 1;
  } else {
    scale = 100;
  }

  float delta = event->angleDelta().y() / scale;
  float distance = m_view_matrix.column(3).z();
  distance += delta;
  m_view_matrix.setColumn(3, QVector4D(m_pan_x, -m_pan_y, distance, 1.f));
  update();
}

void LayoutCanvas::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Control) {
    m_ctrl_pressed = true;
  } else {
    return QOpenGLWidget::keyPressEvent(event);
  }
}

void LayoutCanvas::keyReleaseEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Control) {
    m_ctrl_pressed = false;
  } else {
    return QOpenGLWidget::keyPressEvent(event);
  }
}
