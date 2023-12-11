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
#include <QPainter>
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <QTransform>

class EllipseWidget : public QQuickPaintedItem {
public:
  EllipseWidget(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {
    qDebug() << ("init~\n");
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
  }

protected:
  virtual void mousePressEvent(QMouseEvent *event) override { qDebug() << ("press~\n"); }
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
    qDebug() << "x=" << x << ", A=" << m_angleA;
    update();
  }

protected:
  void paint(QPainter *painter) {
    printf("geometry:x=%f,w=%f", x(), width());
    QRectF bbox(110, 110, 100, 60);
    QPainterPath ellipsePath;
    ellipsePath.addEllipse(bbox);

    QTransform transform;
    QTransform trabefore, traBack;

    qreal centerX = bbox.x() + bbox.width() / 2;
    qreal centerY = bbox.y() + bbox.height() / 2;

    trabefore.translate(-centerX, -centerY);
    transform.rotate(m_angleA, Qt::ZAxis, 0);

    QPolygonF polygon = ellipsePath.toFillPolygon();
    polygon = trabefore.map(polygon); // 1. tras to 00
    polygon = transform.map(polygon); // 2. rotate

    traBack.translate(centerX, centerY); // 3. tras back
    polygon = traBack.map(polygon);

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
  }
  int m_angleA = 40;
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
