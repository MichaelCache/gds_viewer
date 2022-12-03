#include "LayoutCanvas.h"

#include <gdstk.h>

#include <QApplication>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QtGlobal>

#include "../../event/OpenFileEvent.h"
#include "../../event/OpenGdsEvent.h"

LayoutCanvas::LayoutCanvas(QWidget* parent) : QOpenGLWidget(parent) {
  EventDispacher::instance().registComp("LayoutCanvas", this);
}

LayoutCanvas::~LayoutCanvas() {
  // Make sure the context is current and then explicitly
  // destroy all underlying OpenGL resources.
  makeCurrent();
  for (auto&& vao : m_vaos.keys()) {
    if (vao) vao->destroy();
  }
  for (auto&& vbo : m_vbos) {
    if (vbo) vbo->destroy();
  }

  if (m_shader) delete m_shader;
  doneCurrent();
}

void LayoutCanvas::initializeGL() {
  m_shader = new QOpenGLShaderProgram();
  m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                    ":/shader/vertex.vert");
  m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                    ":/shader/fragment.frag");
  if (m_shader->link()) {
    qDebug("Shaders link success.");
  } else {
    qDebug("Shaders link failed!");
  }
  m_shader->bind();
  m_MVP_matrix_id = m_shader->uniformLocation("uMvpMatrix");
  m_shader->release();
  m_gl_func = this->context()->functions();

  m_proj_matrix.perspective(50.f, 1.f, 0.1f, 500.0f);
  m_view_matrix.lookAt(QVector3D(0.f, 0.f, 20.f), QVector3D(0.f, 0.f, 0.f),
                      QVector3D(0.f, 1.f, 0.f));
  m_model_matrix.scale(1.f, 1.f);
  m_view_matrix.setToIdentity();
}

void LayoutCanvas::paintGL() {
  // m_gl_func->glUniformMatrix4fv(viewLoc, 1, GL_FALSE, m_view.data());
  // QOpenGLFunctions* f = this->context()->functions();
  m_gl_func->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_gl_func->glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
  m_shader->bind();

  QMatrix4x4 projViewMatrix = m_proj_matrix * m_view_matrix;
  m_mvp_matrix = projViewMatrix * m_model_matrix;
  m_shader->setUniformValue(m_MVP_matrix_id, m_mvp_matrix);
  // qDebug() << m_mvp_matrix;

  for (auto it = m_vaos.begin(); it != m_vaos.end(); it++) {
    it.key()->bind();
    m_gl_func->glDrawArrays(GL_LINE_LOOP, 0, it.value());
    it.key()->release();
  }
  m_shader->release();
}

void LayoutCanvas::resizeGL(int w, int h) {
  Q_UNUSED(w);
  Q_UNUSED(h);
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
    QString fn = dynamic_cast<OpenFileEvent*>(e)->data();
    gdstk::ErrorCode error_code;
    gdstk::Set<gdstk::Tag> tags = {0};
    m_gds_lib =
        gdstk::read_gds(fn.toLocal8Bit().data(), 0, 1e-2, NULL, &error_code);
    makeCurrent();
    parseGds();
    doneCurrent();
    e->accept();
    return true;
  } else {
    return QOpenGLWidget::event(e);
  }
}
void LayoutCanvas::wheelEvent(QWheelEvent* event) {
  float delta = event->angleDelta().y() / 100.f;
  float distance = m_view_matrix.column(3).z();
  distance += delta;
  m_view_matrix.setColumn(3, QVector4D(m_pan_x, -m_pan_y, distance, 1.f));
  update();
}

void LayoutCanvas::parseGds() {
  for (size_t i = 0; i < m_gds_lib.cell_array.count; i++) {
    gdstk::Cell* cell = m_gds_lib.cell_array[i];

    gdstk::Array<gdstk::Polygon*> polygons = {0};
    cell->get_polygons(true, true, -1, false, 0, polygons);
    for (size_t j = 0; j < polygons.count; j++) {
      makePolygons(polygons[j]);
    }
  }
}

void LayoutCanvas::makePolygons(gdstk::Polygon* polygons) {
  auto vao = new QOpenGLVertexArrayObject();
  m_vaos.insert(vao, polygons->point_array.count);

  auto vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
  m_vbos.append(vbo);
  vao->create();
  vao->bind();

  vbo->create();
  vbo->bind();
  vbo->allocate(polygons->point_array.items,
                polygons->point_array.count * 2 * sizeof(GLdouble));

  m_gl_func->glEnableVertexAttribArray(0);
  m_gl_func->glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE,
                                   2 * sizeof(GLdouble), 0);

  vbo->release();
  vao->release();
}