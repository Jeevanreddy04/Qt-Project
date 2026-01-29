#include "CameraImageProvider.h"

#include "CameraController.h"

#include <QImage>
#include <QSize>

CameraImageProvider::CameraImageProvider(CameraController *controller)
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_controller(controller) {}

QImage CameraImageProvider::requestImage(const QString &, QSize *size,
                                        const QSize &requestedSize) {
  if (!m_controller) {
    QImage placeholder(640, 480, QImage::Format_RGB32);
    placeholder.fill(Qt::black);
    if (size)
      *size = placeholder.size();
    return placeholder;
  }

  QImage img = m_controller->currentFrame();
  if (img.isNull()) {
    QImage placeholder(640, 480, QImage::Format_RGB32);
    placeholder.fill(Qt::black);
    if (size)
      *size = placeholder.size();
    return placeholder;
  }

  if (requestedSize.isValid()) {
    img = img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  if (size)
    *size = img.size();
  return img;
}
