#include "CameraController.h"

#include <algorithm>
#include <QDateTime>

CameraController::CameraController(QObject *parent) : QObject(parent)
{
  m_timer.setTimerType(Qt::PreciseTimer);
  connect(&m_timer, &QTimer::timeout, this, &CameraController::grabFrame);

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
  emit errorOccurred(QStringLiteral("Camera support requires OpenCV library."));
  return false;
}

void CameraController::closeCamera()
{
  if (m_timer.isActive())
    m_timer.stop();
}

void CameraController::applyDesiredResolution()
{
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

  const int targetIndex =
      (latestIndex + m_offset + m_ringSize) % m_ringSize;

  QMutexLocker locker(&m_frameMutex);
  m_displayedFrame = m_ring[targetIndex];
}

void CameraController::grabFrame()
{
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

  const int intervalMs = std::max(1, 1000 / std::max(1, m_targetFps));
  if (!m_timer.isActive())
    m_timer.start(intervalMs);
}

void CameraController::pause()
{
  if (!m_playing)
    return;

  if (m_timer.isActive())
    m_timer.stop();

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

  emit resolutionChanged();
}

