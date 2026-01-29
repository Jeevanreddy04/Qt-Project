#pragma once

#include <QQuickImageProvider>

class CameraController;

class CameraImageProvider final : public QQuickImageProvider {
public:
  explicit CameraImageProvider(CameraController *controller);

  QImage requestImage(const QString &id, QSize *size,
                      const QSize &requestedSize) override;

private:
  CameraController *m_controller = nullptr;
};
