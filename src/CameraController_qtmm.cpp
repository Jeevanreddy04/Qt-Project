#include "CameraController.h"

#include <algorithm>

#include <QCamera>
#include <QCameraDevice>
#include <QCameraFormat>
#include <QMediaDevices>
#include <QVideoFrame>

CameraController::CameraController(QObject *parent) : QObject(parent)
{
  // Keep the existing buffering + transport behavior; frames will be pushed
  // from Qt Multimedia's video sink instead of polling with a timer.
  m_timer.setTimerType(Qt::PreciseTimer);
  connect(&m_timer, &QTimer::timeout, this, &CameraController::grabFrame);

  connect(&m_videoSink, &QVideoSink::videoFrameChanged, this,
          [this](const QVideoFrame &) { onVideoFrame(); });

  resetBuffer();
}

CameraController::~CameraController() { closeCamera(); }

QString CameraController::resolutionText() const
{
  const int w = m_actualWidth > 0 ? m_actualWidth : m_desiredWidth;
  const int h = m_actualHeight > 0 ? m_actualHeight : m_desiredHeight;
  return QString::number(w) + "x" + QString::number(h);
}

QStringList CameraController::availableResolutions() const
{
  // Kept constant for QML simplicity.
  return {
      QStringLiteral("640x480"),
      QStringLiteral("1280x720"),
      QStringLiteral("1920x1080"),
  };
}

void CameraController::setStepFrames(int frames)
{
  const int clamped = std::max(1, frames);
  if (m_stepFrames == clamped)
    return;
  m_stepFrames = clamped;
  emit stepFramesChanged();
}

void CameraController::setBufferSeconds(int seconds)
{
  const int clamped = std::max(1, seconds);
  if (m_bufferSeconds == clamped)
    return;
  m_bufferSeconds = clamped;
  resetBuffer();
  emit bufferSecondsChanged();
}

QImage CameraController::currentFrame()
{
  QMutexLocker locker(&m_frameMutex);
  return m_displayedFrame;
}

bool CameraController::openCamera()
{
  if (m_camera)
    return true;

  const QCameraDevice device = QMediaDevices::defaultVideoInput();
  if (device.isNull())
  {
    emit errorOccurred(QStringLiteral("No camera device found."));
    return false;
  }

  m_camera = new QCamera(device, this);
  m_captureSession.setCamera(m_camera);
  m_captureSession.setVideoSink(&m_videoSink);

  applyDesiredResolution();

  m_camera->start();
  if (!m_camera->isActive())
  {
    emit errorOccurred(QStringLiteral("Failed to start the camera."));
    closeCamera();
    return false;
  }

  const QSize res = m_camera->cameraFormat().resolution();
  m_actualWidth = res.width();
  m_actualHeight = res.height();
  emit resolutionChanged();

  return true;
}

void CameraController::closeCamera()
{
  if (m_timer.isActive())
    m_timer.stop();

  if (m_camera)
  {
    m_camera->stop();
    m_captureSession.setCamera(nullptr);
    m_captureSession.setVideoSink(nullptr);
    delete m_camera;
    m_camera = nullptr;
  }
}

void CameraController::applyDesiredResolution()
{
  if (!m_camera)
    return;

  const auto formats = m_camera->cameraDevice().videoFormats();
  if (formats.isEmpty())
    return;

  auto score = [&](const QCameraFormat &f) -> qint64 {
    const QSize r = f.resolution();
    const qint64 dw = static_cast<qint64>(r.width()) - m_desiredWidth;
    const qint64 dh = static_cast<qint64>(r.height()) - m_desiredHeight;
    return dw * dw + dh * dh;
  };

  QCameraFormat best = formats.first();
  qint64 bestScore = score(best);
  for (const auto &f : formats)
  {
    const qint64 s = score(f);
    if (s < bestScore)
    {
      best = f;
      bestScore = s;
    }
  }

  // Format changes are most reliable while stopped.
  const bool wasActive = m_camera->isActive();
  if (wasActive)
    m_camera->stop();
  m_camera->setCameraFormat(best);
  if (wasActive)
    m_camera->start();
}

void CameraController::resetBuffer()
{
  const int size = std::max(30, m_targetFps * m_bufferSeconds);
  m_ringSize = size;
  m_ring = QVector<QImage>(m_ringSize);
  m_head = 0;
  m_count = 0;
  m_offset = 0;
}

void CameraController::pushFrame(const QImage &img)
{
  if (img.isNull())
    return;

  m_ring[m_head] = img;
  m_head = (m_head + 1) % m_ringSize;
  m_count = std::min(m_count + 1, m_ringSize);
}

void CameraController::updateDisplayedFromOffset()
{
  if (m_count <= 0)
    return;

  const int latestIndex = (m_head - 1 + m_ringSize) % m_ringSize;
  const int minOffset = -m_count + 1;
  m_offset = std::min(0, std::max(minOffset, m_offset));

  const int targetIndex = (latestIndex + m_offset + m_ringSize) % m_ringSize;

  QMutexLocker locker(&m_frameMutex);
  m_displayedFrame = m_ring[targetIndex];
}

void CameraController::onVideoFrame()
{
  if (!m_camera || m_paused)
    return;

  const QVideoFrame frame = m_videoSink.videoFrame();
  if (!frame.isValid())
    return;

  const QImage img = frame.toImage();
  if (img.isNull())
    return;

  pushFrame(img);
  m_offset = 0;
  updateDisplayedFromOffset();
  emit frameUpdated();
}

void CameraController::grabFrame()
{
  // Not used for Qt Multimedia backend; frames arrive via QVideoSink.
}

void CameraController::play()
{
  if (!openCamera())
    return;

  if (!m_playing)
  {
    m_playing = true;
    emit playingChanged();
  }

  if (m_paused)
  {
    m_paused = false;
    emit pausedChanged();
  }
}

void CameraController::pause()
{
  if (!m_playing)
    return;

  if (!m_paused)
  {
    m_paused = true;
    emit pausedChanged();
  }
}

void CameraController::resume() { play(); }

void CameraController::forward()
{
  if (!m_paused || m_count <= 0)
    return;

  m_offset = std::min(0, m_offset + m_stepFrames);
  updateDisplayedFromOffset();
  emit frameUpdated();
}

void CameraController::rewind()
{
  if (!m_paused || m_count <= 0)
    return;

  const int minOffset = -m_count + 1;
  m_offset = std::max(minOffset, m_offset - m_stepFrames);
  updateDisplayedFromOffset();
  emit frameUpdated();
}

void CameraController::setResolutionString(const QString &res)
{
  const auto parts = res.split('x');
  if (parts.size() != 2)
  {
    emit errorOccurred(QStringLiteral("Invalid resolution format."));
    return;
  }

  bool okW = false;
  bool okH = false;
  const int w = parts[0].toInt(&okW);
  const int h = parts[1].toInt(&okH);
  if (!okW || !okH || w <= 0 || h <= 0)
  {
    emit errorOccurred(QStringLiteral("Invalid resolution values."));
    return;
  }

  m_desiredWidth = w;
  m_desiredHeight = h;

  if (m_camera)
  {
    applyDesiredResolution();
    const QSize actual = m_camera->cameraFormat().resolution();
    m_actualWidth = actual.width();
    m_actualHeight = actual.height();
  }

  emit resolutionChanged();
}

