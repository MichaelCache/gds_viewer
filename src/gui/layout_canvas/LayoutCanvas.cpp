#include "LayoutCanvas.h"

#include <gdstk.h>

#include <QApplication>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QtGlobal>

#include "../../event/CloseFileEvent.h"
#include "../../event/OpenFileEvent.h"
#include "../../event/OpenGdsEvent.h"
#include "Triangulate.h"

namespace {
const QVector3D DEFAULT_LAYER_COLOR[] = {
    {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
};
}  // namespace

LayoutCanvas::LayoutCanvas(QWidget* parent) : QOpenGLWidget(parent) {
  EventDispacher::instance().registComp("LayoutCanvas", this);
  setFocusPolicy(Qt::WheelFocus);
}

LayoutCanvas::~LayoutCanvas() {
  // Make sure the context is current and then explicitly
  // destroy all underlying OpenGL resources.
  makeCurrent();
  for (auto&& vao : m_border_vaos.keys()) {
    if (vao) vao->destroy();
  }
  for (auto&& vbo : m_border_vbos) {
    if (vbo) vbo->destroy();
  }

  for (auto&& vao : m_fill_vaos.keys()) {
    if (vao) vao->destroy();
  }
  for (auto&& vbo : m_fill_vbos) {
    if (vbo) vbo->destroy();
  }

  if (m_border_shader) delete m_border_shader;
  if (m_fill_shader) delete m_fill_shader;
  doneCurrent();
}

void LayoutCanvas::initializeGL() {
  // border shader
  m_border_shader = new QOpenGLShaderProgram();
  m_border_shader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shader/vertex.vert");
  m_border_shader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shader/border.frag");
  if (m_border_shader->link()) {
    qDebug("Shaders link success.");
  } else {
    qDebug("Shaders link failed!");
  }
  m_border_shader->bind();

  m_modelToWorld = m_border_shader->uniformLocation("modelToWorld");
  m_worldToCamera = m_border_shader->uniformLocation("worldToCamera");
  m_cameraToView = m_border_shader->uniformLocation("cameraToView");
  m_color = m_border_shader->uniformLocation("color");

  m_border_shader->release();

  // fill shader
  m_fill_shader = new QOpenGLShaderProgram();
  m_fill_shader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                         ":/shader/vertex.vert");
  m_fill_shader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                         ":/shader/fill.frag");
  if (m_fill_shader->link()) {
    qDebug("Shaders link success.");
  } else {
    qDebug("Shaders link failed!");
  }
  m_fill_shader->bind();
  // m_MVP_matrix_id = m_shader->uniformLocation("uMvpMatrix");
  m_modelToWorld = m_fill_shader->uniformLocation("modelToWorld");
  m_worldToCamera = m_fill_shader->uniformLocation("worldToCamera");
  m_cameraToView = m_fill_shader->uniformLocation("cameraToView");
  m_color = m_fill_shader->uniformLocation("color");

  m_fill_shader->release();

  m_gl_func = this->context()->functions();
  // enable alpha blend
  m_gl_func->glEnable(GL_BLEND);
  m_gl_func->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_view_matrix.lookAt(QVector3D(0.f, 0.f, 0.f), QVector3D(0.f, 0.f, 0.f),
                       QVector3D(0.f, 1.f, 0.f));
  m_model_matrix.scale(1.f, 1.f);
}

void LayoutCanvas::paintGL() {
  // m_gl_func->glUniformMatrix4fv(viewLoc, 1, GL_FALSE, m_view.data());
  // QOpenGLFunctions* f = this->context()->functions();
  m_gl_func->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_gl_func->glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
  m_border_shader->bind();

  m_border_shader->setUniformValue(m_modelToWorld, m_model_matrix);
  m_border_shader->setUniformValue(m_worldToCamera, m_view_matrix);
  m_border_shader->setUniformValue(m_cameraToView, m_proj_matrix);

  for (auto it = m_border_vaos.begin(); it != m_border_vaos.end(); it++) {
    it.key()->bind();
    m_border_shader->setUniformValue(m_color, it.value().second);
    m_gl_func->glDrawArrays(GL_LINE_LOOP, 0, it.value().first);
    it.key()->release();
  }
  m_border_shader->release();

  m_fill_shader->bind();

  m_fill_shader->setUniformValue(m_modelToWorld, m_model_matrix);
  m_fill_shader->setUniformValue(m_worldToCamera, m_view_matrix);
  m_fill_shader->setUniformValue(m_cameraToView, m_proj_matrix);
  for (auto it = m_fill_vaos.begin(); it != m_fill_vaos.end(); it++) {
    it.key()->bind();
    m_fill_shader->setUniformValue(m_color, it.value().second);
    m_gl_func->glDrawArrays(GL_TRIANGLES, 0, it.value().first);
    it.key()->release();
  }
  m_fill_shader->release();
}

void LayoutCanvas::resizeGL(int w, int h) {
  m_proj_matrix.setToIdentity();
  m_proj_matrix.perspective(45.0f, w / float(h), 0.0f, 1000.0f);
}

void LayoutCanvas::mouseMoveEvent(QMouseEvent* event) {
  if (!m_panning) {
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
    case Qt::MiddleButton: {
      if (m_panning) {
        return;
      }
      m_panning = true;
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

  if (!m_panning) {
    return;
  }
  m_panning = false;
}

bool LayoutCanvas::event(QEvent* e) {
  if (e->type() == OpenFileEvent::type) {
    clear();
    QString fn = dynamic_cast<OpenFileEvent*>(e)->data();
    gdstk::ErrorCode error_code;
    gdstk::Set<gdstk::Tag> tags = {0};
    auto lib =
        gdstk::read_gds(fn.toLocal8Bit().data(), 0, 1e-2, NULL, &error_code);
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

void LayoutCanvas::parseGds(const gdstk::Library& lib) {
  for (size_t i = 0; i < lib.cell_array.count; i++) {
    gdstk::Cell* cell = lib.cell_array[i];

    gdstk::Array<gdstk::Polygon*> polygons = {0};
    cell->get_polygons(true, true, -1, false, 0, polygons);
    for (size_t j = 0; j < polygons.count; j++) {
      makePolygons(polygons[j]);
    }
    polygons.clear();
  }
}

void LayoutCanvas::makePolygons(gdstk::Polygon* polygons) {
  makeCurrent();
  // fill
  {
    // triangluate, divide polygon to triangles
    Vector2dVector p;
    for (size_t i = 0; i < polygons->point_array.count; i++) {
      p.push_back(
          (Vector2d(polygons->point_array[i].x, polygons->point_array[i].y)));
    }
    Vector2dVector result;
    Triangulate::Process(p, result);

    auto fill_vao = new QOpenGLVertexArrayObject();
    uint32_t layer = gdstk::get_layer(polygons->tag);

    m_fill_vaos.insert(fill_vao, {result.size(), DEFAULT_LAYER_COLOR[layer]});

    auto fill_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_fill_vbos.append(fill_vbo);
    fill_vao->create();
    fill_vao->bind();

    fill_vbo->create();
    fill_vbo->bind();
    fill_vbo->allocate(result.data(), result.size() * 2 * sizeof(GLfloat));

    m_gl_func->glEnableVertexAttribArray(0);
    m_gl_func->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                     2 * sizeof(GLfloat), 0);

    fill_vbo->release();
    fill_vao->release();
  }
  // border
  {
    auto border_vao = new QOpenGLVertexArrayObject();
    uint32_t layer = gdstk::get_layer(polygons->tag);

    m_border_vaos.insert(
        border_vao, {polygons->point_array.count, DEFAULT_LAYER_COLOR[layer]});

    auto border_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_border_vbos.append(border_vbo);
    border_vao->create();
    border_vao->bind();

    border_vbo->create();
    border_vbo->bind();
    border_vbo->allocate(polygons->point_array.items,
                         polygons->point_array.count * 2 * sizeof(GLdouble));

    m_gl_func->glEnableVertexAttribArray(0);
    m_gl_func->glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE,
                                     2 * sizeof(GLdouble), 0);

    border_vbo->release();
    border_vao->release();
  }

  doneCurrent();
}

void LayoutCanvas::clear() {
  makeCurrent();
  for (auto&& vao : m_border_vaos.keys()) {
    if (vao) vao->destroy();
  }
  for (auto&& vbo : m_border_vbos) {
    if (vbo) vbo->destroy();
  }

  for (auto&& vao : m_fill_vaos.keys()) {
    if (vao) vao->destroy();
  }
  for (auto&& vbo : m_fill_vbos) {
    if (vbo) vbo->destroy();
  }
  m_border_vaos.clear();
  m_border_vbos.clear();
  m_fill_vaos.clear();
  m_fill_vbos.clear();
  update();
  doneCurrent();
}