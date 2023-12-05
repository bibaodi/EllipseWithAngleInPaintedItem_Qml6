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
#include <QPainter>
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <QTransform>

class EllipseWidget : public QQuickPaintedItem {
public:
  EllipseWidget(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {}

protected:
  void paint(QPainter *painter) {
    QPainterPath ellipsePath;
    ellipsePath.addEllipse(100, 100, 50, 30);

    QTransform transform;
    transform.rotate(30);

    QPolygonF polygon = ellipsePath.toFillPolygon();
    polygon = transform.map(polygon);
    painter->drawPolygon(polygon);
  }
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
  EllipseWidget elli;

  return app.exec();
}
