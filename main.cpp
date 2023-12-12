#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#else
#endif

/*

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/drawEllpise/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

*/
//#include <QGui/QGuiApplication>
#include <QDebug>
#include <QLineF>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QQuickPaintedItem>
#include <QTransform>
#include <math.h>

class EllipseShape {
public:
  EllipseShape() : m_ready(false), m_fillPtNum(0) {}
  bool getAngleWithXAxix(qreal &angleDegree) {
    angleDegree = 0.f;
    if (false == m_ready) {
      return false;
    }
    const QLineF &slip = m_longDiameter;
    QPointF pt1 = slip.p1();
    QPointF pt2 = slip.p2();
    /* if (pt2.x() < pt1.x()) {
          pt2 = pt1;
          pt1 = slip.p2();
        }*/
    float deltaX = pt2.x() - pt1.x();
    float deltaY = -(pt2.y() - pt1.y());

    angleDegree = std::atan2(deltaY, deltaX);
    angleDegree = qRadiansToDegrees(angleDegree);
    qDebug() << "angle=" << angleDegree << ", line=" << m_longDiameter << ",qAngle=" << m_longDiameter.angle();
    return true;
  }
  qreal getAngle2() { return -1.f * m_longDiameter.angle(); }
  void addPoint(QPoint pt) {
    switch (m_fillPtNum) {
    case 0: {
      m_longDiameter.setP1(pt);
      m_fillPtNum++;
      break;
    }
    default:
    case 1: {
      m_longDiameter.setP2(pt);
      m_fillPtNum++;
      m_ready = true;
      setShortDiameter();
      break;
    }
    }
  }
  void addPoint(QPointF pt) { return addPoint(pt.toPoint()); }
  void setShortDiameter() {
    // QPoint center = m_longDiameter.center();
    m_shortDiameterLength = m_longDiameter.length();
  }
  bool isReady() const { return m_ready; }
  QLineF getLongDia() const { return m_longDiameter; }

  QRectF getRect() {
    QRectF rect;
    rect.setWidth(m_longDiameter.length());
    rect.setHeight(rect.width() * 0.5);
    rect.moveCenter(m_longDiameter.center());

    return rect;
  }

private:
  QLineF m_longDiameter;
  qreal m_shortDiameterLength;
  bool m_ready;
  int m_fillPtNum;
};

//!
//! \brief transformEllipse rotate a ellipse defined by rect with angleA;
//! \param bbox
//! \param angleA
//! \param rpolygon
//! \return
//!
int transformEllipse(QRectF bbox, qreal angleA, QPolygonF &rpolygon) {
  QPainterPath ellipsePath;
  ellipsePath.addEllipse(bbox);

  QTransform transform;
  QTransform trabefore, traBack;

  qreal centerX = bbox.x() + bbox.width() / 2;
  qreal centerY = bbox.y() + bbox.height() / 2;

  trabefore.translate(-centerX, -centerY);
  transform.rotate(angleA, Qt::ZAxis, 0);

  QPolygonF polygon = ellipsePath.toFillPolygon();
  polygon = trabefore.map(polygon); // 1. tras to 00
  polygon = transform.map(polygon); // 2. rotate

  traBack.translate(centerX, centerY); // 3. tras back
  polygon = traBack.map(polygon);
  rpolygon = polygon;

  return 0;
}

class EllipseWidget : public QQuickPaintedItem {
public:
  EllipseWidget(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {
    qDebug() << ("init~\n");
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
  }

protected:
  virtual void mousePressEvent(QMouseEvent *event) override {
    qDebug() << ("press~\n");
    m_elli.addPoint(event->position());
    qreal angle;
    bool ret = m_elli.getAngleWithXAxix(angle);
    qDebug() << "ret=" << ret << ", angle=" << angle;
    update();
  }
  virtual void mouseMoveEvent(QMouseEvent *event) override { printf("mouse movex=%d\n", event->position().x()); }
  virtual void mouseReleaseEvent(QMouseEvent *event) override { printf("mouse rx=%d\n", event->position().x()); }
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override {
    printf("mouse dmovex=%d\n", event->position().x());
  }
  virtual void wheelEvent(QWheelEvent *ev) {
    int x = ev->angleDelta().y();
    x = x / 120;
    m_angleA += x;
    m_angleA = std::min(std::max(0, m_angleA), 360);
    if (0 == m_angleA && x < 0) {
      m_angleA = 360;
    }
    if (360 == m_angleA && x > 0) {
      m_angleA = 0;
    }
    qDebug() << "wheel Event:x=" << x << ", A=" << m_angleA;
    update();
  }

protected:
  void paint(QPainter *painter) {
    QRectF bbox(110, 110, 100, 60);
    if (false == m_elli.isReady()) {
      return;
    }
    bbox = m_elli.getRect();
    QPolygonF polygon;
    transformEllipse(bbox, m_elli.getAngle2(), polygon);

    QString msg = QString("Angle={%1}Degree").arg(m_angleA);

    QPen myPen;
    myPen.setColor(Qt::blue);
    myPen.setWidth(2);
    painter->setPen(myPen);
    painter->drawPolygon(polygon);
    myPen.setColor(Qt::red);
    painter->setPen(myPen);
    painter->drawRect(bbox);
    painter->drawText(70, 70, msg);

    if (m_elli.isReady()) {
      painter->drawLine(m_elli.getLongDia());
    }
  }
  int m_angleA = 40;
  EllipseShape m_elli;
};

int main(int argc, char *argv[]) {
  qmlRegisterType<EllipseWidget>("com.acr.aoivus", 1, 0, "EllipseWidgetQml");
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  const QUrl url(u"qrc:/drawEllpise/Main.qml"_qs);
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
