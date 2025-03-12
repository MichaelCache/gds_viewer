#include "LayoutCanvas.h"

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QTime>
#include <QWheelEvent>
#include <QtGlobal>

#include "Triangulate.h"
#include "event/CloseFileEvent.h"
#include "event/CurrentFPSEvent.h"
#include "event/EventDispacher.h"
#include "event/OpenFileEvent.h"
#include "event/SetCellNamesEvent.h"

namespace {
// clang-format off
const QVector<QVector3D> DEFAULT_LAYER_COLOR = {
    QVector3D(0.0f, 0.0f, 0.0f), 
    QVector3D(0.5f, 0.0f, 0.0f), 
    QVector3D(0.0f, 0.5f, 0.0f),
    QVector3D(0.0f, 0.0f, 0.5f), 
    QVector3D(0.0f, 0.0f, 1.0f), 
    QVector3D(0.0f, 1.0f, 0.0f),
    QVector3D(1.0f, 0.0f, 0.0f), 
    QVector3D(0.0f, 0.5f, 0.5f), 
    QVector3D(0.5f, 0.5f, 0.0f),
    QVector3D(0.5f, 0.0f, 0.5f), 
    QVector3D(0.0f, 0.5f, 1.0f), 
    QVector3D(0.0f, 1.0f, 0.5f),
    QVector3D(0.0f, 1.0f, 1.0f), 
    QVector3D(0.5f, 0.0f, 1.0f), 
    QVector3D(0.5f, 0.5f, 0.5f),
    QVector3D(0.5f, 0.5f, 1.0f), 
    QVector3D(0.5f, 1.0f, 0.0f), 
    QVector3D(0.5f, 1.0f, 0.5f),
    QVector3D(0.5f, 1.0f, 1.0f), 
    QVector3D(1.0f, 0.0f, 0.5f), 
    QVector3D(1.0f, 0.0f, 1.0f),
    QVector3D(1.0f, 0.5f, 0.0f), 
    QVector3D(1.0f, 0.5f, 0.5f), 
    QVector3D(1.0f, 0.5f, 1.0f),
    QVector3D(1.0f, 1.0f, 0.0f), 
    QVector3D(1.0f, 1.0f, 0.5f), 
    QVector3D(1.0f, 1.0f, 1.0f),
};
// clang-format on

}  // namespace

LayoutCanvas::CellPolygonVertex::CellPolygonVertex() {
  vertex_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  color_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
}

void LayoutCanvas::CellPolygonVertex::clear() {
  for (auto&& i : polygon_vaos) {
    if (i.first->isCreated()) {
      i.first->destroy();
    }
    delete i.first;
  }
  polygon_vaos.clear();
  if (vertex_vbo->isCreated()) {
    vertex_vbo->destroy();
  }

  delete vertex_vbo;
  if (color_vbo->isCreated()) {
    color_vbo->destroy();
  }
  delete color_vbo;
}

LayoutCanvas::LayoutCanvas(QWidget* parent) : QOpenGLWidget(parent) {
  EventDispacher::instance().registComp(EventComp::LayoutCanvas, this);
  setFocusPolicy(Qt::WheelFocus);
  m_last_time = QTime::currentTime();
}

LayoutCanvas::~LayoutCanvas() {
  // Make sure the context is current and then explicitly
  // destroy all underlying OpenGL resources.
  clear();
  if (m_shader_prog) {
    delete m_shader_prog;
    m_shader_prog = nullptr;
  }
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
  m_shader_prog = new QOpenGLShaderProgram();
  m_shader_prog->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                         ":/shader/vertex.glsl");
  m_shader_prog->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                         ":/shader/fragment.glsl");
  if (m_shader_prog->link()) {
    qDebug() << "Shaders link success.";
  } else {
    qDebug() << "Shaders link failed!";
  }

  m_view_matrix.lookAt(QVector3D(0.f, 0.f, 0.f), QVector3D(0.f, 0.f, 0.f),
                       QVector3D(0.f, 1.f, 0.f));
  m_proj_matrix.setToIdentity();
  // TODO:draw background mesh
  // initializeGrid();
  // initialize color buffer
  // makeCurrent();
  // color_vbo->create();
  // color_vbo->bind();
  // color_vbo->allocate(DEFAULT_LAYER_COLOR, sizeof(DEFAULT_LAYER_COLOR));
  // color_vbo->release();
  // doneCurrent();
}

void LayoutCanvas::paintGL() {
  // clean background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // set background color
  glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
  m_shader_prog->bind();
  // modelToWorld Matrix is identity
  m_shader_prog->setUniformValue("viewMatrix", m_view_matrix);
  m_shader_prog->setUniformValue("projMatrix", m_proj_matrix);

  if (m_cellname_vertex.count(m_current_cellname)) {
    auto& cell_vertex = m_cellname_vertex[m_current_cellname];
    for (auto&& i : cell_vertex.polygon_vaos) {
      i.first->bind();
      // draw polygon border
      glLineWidth(3.0f);
      glDrawArrays(GL_LINE_LOOP, 0, i.second);
      i.first->release();
    }
  }

  m_shader_prog->release();
  calculateFPS();
}

void LayoutCanvas::resizeGL(int w, int h) {
  float width = this->width();
  float height = this->height();
  float y_x_ratio = height / width;

  m_proj_matrix = QMatrix4x4(
      // line 1
      y_x_ratio * m_scale, 0, 0, 0,
      // line 2
      0, m_scale, 0, 0,
      // line 3
      0, 0, 1, 0,
      // line 4
      0, 0, 0, 1);
  update();
}

void LayoutCanvas::initializeGrid() {
  float gridSize = 1.0f;
  int gridLines = 10;
  QVector<float> gridVertices;
  for (int i = 0; i <= gridLines; ++i) {
    float pos = -gridSize + (2.0f * gridSize / gridLines) * i;

    // vertical line
    gridVertices << -gridSize << pos << 0.0f;
    gridVertices << gridSize << pos << 0.0f;

    // horizontal line
    gridVertices << pos << -gridSize << 0.0f;
    gridVertices << pos << gridSize << 0.0f;
  }

  // vao1.create();
  // vao1.bind();

  // vbo.create();
  // vbo.bind();
  // vbo.allocate(gridVertices.data(), gridVertices.size() * sizeof(float));

  m_shader_prog->enableAttributeArray(0);
  m_shader_prog->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));

  // vbo.release();
  // vao1.release();
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

    gdstk::Vec2 min = {};
    gdstk::Vec2 max = {};
    cell->bounding_box(min, max);
    double box_width = max.x - min.x;
    double box_height = max.y - min.y;
    m_scale = std::min(2 / box_width, 2 / box_height);
    float width = this->width();
    float height = this->height();
    float y_x_ratio = height / width;

    // use orthogonal project matrix scale to fit the screen
    m_proj_matrix = QMatrix4x4(
        // line 1
        y_x_ratio * m_scale, 0, 0, 0,
        // line 2
        0, m_scale, 0, 0,
        // line 3
        0, 0, 1, 0,
        // line 4
        0, 0, 0, 1);

    double center_x = min.x + box_width / 2;
    double center_y = min.y + box_height / 2;
    // use view matrix translate to cell center
    m_view_matrix.translate(-center_x, -center_y);
  }
  update();
}

void LayoutCanvas::makeCellPolygons(gdstk::Cell* cell) {
  makeCurrent();
  // recrusively get all polygons of current cell
  gdstk::Array<gdstk::Polygon*> polygons = {0};
  cell->get_polygons(true, true, -1, false, 0, polygons);

  auto& cell_vertex = m_cellname_vertex[cell->name];

  QVector<GLfloat> all_vertex;
  QVector<GLfloat> vertex_color;
  for (size_t i = 0; i < polygons.count; i++) {
    auto gds_polygon = polygons[i];
    uint32_t layer = gdstk::get_layer(gds_polygon->tag);
    auto& color = DEFAULT_LAYER_COLOR.at(layer % DEFAULT_LAYER_COLOR.size());

    for (size_t i = 0; i < gds_polygon->point_array.count; i++) {
      auto& point = gds_polygon->point_array[i];
      all_vertex.push_back(point.x);
      all_vertex.push_back(point.y);
      vertex_color.push_back(color.x());
      vertex_color.push_back(color.y());
      vertex_color.push_back(color.z());
    }
  }
  // TODO:triangluate, divide polygon to triangles

  // bind all vertex to one buffer
  cell_vertex.vertex_vbo->create();
  cell_vertex.vertex_vbo->bind();
  cell_vertex.vertex_vbo->allocate(all_vertex.data(),
                                   all_vertex.size() * sizeof(GLfloat));
  cell_vertex.vertex_vbo->release();

  cell_vertex.color_vbo->create();
  cell_vertex.color_vbo->bind();
  cell_vertex.color_vbo->allocate(vertex_color.data(),
                                  vertex_color.size() * sizeof(GLfloat));
  cell_vertex.color_vbo->release();

  int point_offset = 0;

  for (size_t i = 0; i < polygons.count; i++) {
    auto gds_polygon = polygons[i];

    auto vertex_vao = new QOpenGLVertexArrayObject();
    vertex_vao->create();
    vertex_vao->bind();
    // bind vertex
    cell_vertex.vertex_vbo->bind();

    m_shader_prog->bind();
    m_shader_prog->enableAttributeArray(0);
    m_shader_prog->setAttributeBuffer(0, GL_FLOAT,
                                      point_offset * 2 * sizeof(float), 2);
    m_shader_prog->release();

    cell_vertex.vertex_vbo->release();
    vertex_vao->release();
    // bind vertex color
    vertex_vao->bind();
    cell_vertex.color_vbo->bind();

    m_shader_prog->bind();
    m_shader_prog->enableAttributeArray(1);
    m_shader_prog->setAttributeBuffer(1, GL_FLOAT,
                                      point_offset * 3 * sizeof(float), 3);
    m_shader_prog->release();

    cell_vertex.color_vbo->release();
    vertex_vao->release();
    cell_vertex.polygon_vaos.emplace_back(vertex_vao,
                                          gds_polygon->point_array.count);
    point_offset += gds_polygon->point_array.count;
  }

  polygons.clear();
  doneCurrent();
}

void LayoutCanvas::clear() {
  makeCurrent();

  for (auto&& i : m_cellname_vertex) {
    i.clear();
  }

  m_cellname_vertex.clear();
  m_view_matrix.setToIdentity();
  m_view_matrix.lookAt(QVector3D(0.f, 0.f, 0.f), QVector3D(0.f, 0.f, 0.f),
                       QVector3D(0.f, 1.f, 0.f));
  m_proj_matrix.setToIdentity();
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

  double move_x = x - m_prev_x;
  double move_y = y - m_prev_y;

  m_prev_x = x;
  m_prev_y = y;

  float width = this->width();
  float height = this->height();
  float y_x_ratio = height / width;

  m_view_matrix.translate(move_x * 2 / (width * y_x_ratio * m_scale),
                          -move_y * 2 / (height * m_scale));
  update();
}

void LayoutCanvas::mousePressEvent(QMouseEvent* event) {
  switch (event->button()) {
    case Qt::LeftButton: {
      if (m_mouse_bt_hold) {
        return;
      }
      m_mouse_bt_hold = true;
      m_prev_x = event->pos().x();
      m_prev_y = event->pos().y();
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
  float scale = 1;
  float offset = 0;
  if (m_ctrl_pressed) {
    offset = 0.02;
  } else {
    offset = 0.1;
  }

  if (event->angleDelta().y() > 0) {
    scale -= offset;
  } else {
    scale += offset;
  }
  m_scale *= scale;
  m_proj_matrix.scale(scale, scale, 1);
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

void LayoutCanvas::calculateFPS() {
  m_frames++;
  QTime currentTime = QTime::currentTime();
  int deltaTime = m_last_time.msecsTo(currentTime);
  if (deltaTime >= 1000) {
    double fps = m_frames / (deltaTime / 1000.0f);
    m_frames = 0;
    m_last_time = currentTime;
    QApplication::postEvent(
        EventDispacher::instance().getComp(EventComp::MainStatusBar),
        new CurrentFPSEvent(fps));
  }
}
