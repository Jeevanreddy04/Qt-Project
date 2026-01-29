#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "CameraController.h"
#include "CameraImageProvider.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  CameraController cameraController;
  engine.addImageProvider(QStringLiteral("camera"),
                          new CameraImageProvider(&cameraController));
  engine.rootContext()->setContextProperty(QStringLiteral("cameraController"),
                                           &cameraController);

  engine.loadFromModule(QStringLiteral("CameraPlayer"), QStringLiteral("Main"));
  if (engine.rootObjects().isEmpty()) {
    return 1;
  }

  return app.exec();
}

