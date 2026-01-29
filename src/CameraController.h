#pragma once

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QTimer>
#include <QStringList>

#if defined(CAMERA_HAS_OPENCV)
#include <opencv2/opencv.hpp>
#endif
#if defined(CAMERA_HAS_QT_MULTIMEDIA)
#include <QMediaCaptureSession>
#include <QVideoSink>
#endif

class CameraController final : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
  Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)
  Q_PROPERTY(QString resolutionText READ resolutionText NOTIFY resolutionChanged)
  Q_PROPERTY(QStringList availableResolutions READ availableResolutions CONSTANT)
  Q_PROPERTY(int stepFrames READ stepFrames WRITE setStepFrames NOTIFY stepFramesChanged)
  Q_PROPERTY(int bufferSeconds READ bufferSeconds WRITE setBufferSeconds NOTIFY bufferSecondsChanged)

public:
  explicit CameraController(QObject *parent = nullptr);
  ~CameraController() override;

  bool playing() const { return m_playing; }
  bool paused() const { return m_paused; }

  QString resolutionText() const;
  QStringList availableResolutions() const;

  int stepFrames() const { return m_stepFrames; }
  void setStepFrames(int frames);

  int bufferSeconds() const { return m_bufferSeconds; }
  void setBufferSeconds(int seconds);

  // Called by the image provider
  QImage currentFrame();

  Q_INVOKABLE void play();
  Q_INVOKABLE void pause();
  Q_INVOKABLE void resume();
  Q_INVOKABLE void forward();
  Q_INVOKABLE void rewind();
  Q_INVOKABLE void setResolutionString(const QString &res);

signals:
  void frameUpdated();
  void errorOccurred(const QString &message);
  void playingChanged();
  void pausedChanged();
  void resolutionChanged();
  void stepFramesChanged();
  void bufferSecondsChanged();

private slots:
  void grabFrame();

private:
  bool openCamera();
  void closeCamera();
  void applyDesiredResolution();
  void resetBuffer();

#if defined(CAMERA_HAS_OPENCV)
  static QImage matToQImage(const cv::Mat &mat);
#endif

  void pushFrame(const QImage &img);
  void updateDisplayedFromOffset();

  // Capture state
#if defined(CAMERA_HAS_OPENCV)
  cv::VideoCapture m_cap;
#endif
#if defined(CAMERA_HAS_QT_MULTIMEDIA)
  void onVideoFrame();
  QMediaCaptureSession m_captureSession;
  QVideoSink m_videoSink;
  class QCamera *m_camera = nullptr;
#endif
  QTimer m_timer;
  bool m_playing = false;
  bool m_paused = false;

  // Desired / actual resolution
  int m_desiredWidth = 640;
  int m_desiredHeight = 480;
  int m_actualWidth = 0;
  int m_actualHeight = 0;

  // Frame buffer / display
  mutable QMutex m_frameMutex;
  QImage m_displayedFrame;

  QVector<QImage> m_ring;
  int m_ringSize = 0;
  int m_head = 0;   // next write position
  int m_count = 0;  // number of valid frames
  int m_offset = 0; // 0 = latest, negative = older

  // Tuning
  int m_targetFps = 30;
  int m_stepFrames = 10;
  int m_bufferSeconds = 5;
};
