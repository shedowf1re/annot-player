﻿// vlccore/httpstreamsession.cc
// 3/14/2012

#include "module/vlccore/httpstreamsession.h"
#include "module/stream/securebufferedfifostream.h"
#include "module/stream/bufferedremotestream.h"
#include "module/mediacodec/flvmerge.h"
#include "module/mediacodec/flvcodec.h"
#include "module/qtext/filesystem.h"
#include "module/qtext/algorithm.h"
#include <QDesktopServices>
#include <QApplication>
#include <QtCore>
#include <QtNetwork>
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#else
#  include <unistd.h>
#endif // Q_OS_WIN

#define DEBUG "httpstreamsession"
#include "module/debug/debug.h"

// - Progress -

namespace { // anonymous

  void sleep_(uint msecs_)
  {
#ifdef Q_OS_WIN
    QtWin::sleep(msecs_);
#else
    ::sleep(msecs_ / 1000);
#endif // Q_OS_WIN
  }

  class ProgressTask_ : public StoppableTask
  {
    enum { SleepInterval = 1000 }; // wake up every 1 second

    HttpStreamSession *session_;
    bool stop_;
  public:
    explicit ProgressTask_(HttpStreamSession *session)
      : session_(session), stop_(false) { Q_ASSERT(session_); }

    virtual void stop() ///< \override
    { stop_ = true; }

    virtual void run() ///< \override
    {
      while (!stop_ && session_->isRunning()) {
        session_->updateProgress();
        sleep_(SleepInterval);
      }
    }
  };

} // anonymous namespace

// - Construction -

HttpStreamSession::~HttpStreamSession()
{
  if (progressTask_)
    progressTask_->stop();
  foreach (InputStream *in, ins_)
    delete in;
  if (fifo_)
    delete fifo_;
  if (merger_)
    delete merger_;

  //foreach (QObject *obj, deleteLater_)
  //  delete obj;
}

// - Properties -

qint64
HttpStreamSession::availableDuration() const
{ return merger_ ? merger_->timestamp() : 0; }

qint64
HttpStreamSession::receivedSize() const
{
  if (ins_.isEmpty())
    return 0;

  qint64 ret = 0;
  foreach (InputStream *in, ins_) {
    BufferedRemoteStream *r = dynamic_cast<BufferedRemoteStream *>(in);
    Q_ASSERT(r);
    if (r)
      ret += r->availableSize();
  }
  return ret;
}

QString
HttpStreamSession::contentType() const
{
  if (ins_.isEmpty())
    return QString();

  RemoteStream *in = dynamic_cast<RemoteStream *>(ins_.first());
  Q_ASSERT(in);
  return in ? in->contentType() : QString();
}

void
HttpStreamSession::invalidateSize()
{
  if (!fifo_ || ins_.isEmpty())
    return;
  qint64 size = 0;
  foreach (InputStream *in, ins_) {
    RemoteStream *r = dynamic_cast<RemoteStream *>(in);
    Q_ASSERT(r);
    if (r)
      size += r->size();
  }

  if (size)
    fifo_->setSize(size);
  DOUT("size =" << size);
}

void
HttpStreamSession::invalidateFileName()
{
  //bool mp4 = contentType().contains("mp4", Qt::CaseInsensitive);
  QString suf = ".flv";
  QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/Annot";
  fileName_ = dir + "/" + QtExt::escapeFileName(mediaTitle()) + suf;
}

// - Actions -

void
HttpStreamSession::save()
{
  DOUT("enter");
  if (fileName_.isEmpty() || !fifo_ || !fifo_->availableSize()) {
    DOUT("exit: empty fileName or fifo");
    return;
  }

  QFile::remove(fileName_);
  QDir().mkpath(QFileInfo(fileName_).absolutePath());

  bool ok = fifo_->writeFile(fileName_);
  if (!ok || !FlvCodec::isFlvFile(fileName_)) {
    QFile::remove(fileName_);
    emit error(tr("download failed") + ": " + fileName_);
    DOUT("exit: fifo failed to write to file");
    return;
  }

  ok = FlvCodec::updateFlvFileMeta(fileName_);
  if (!ok)
    DOUT("warning: failed to update FLV meta:" << fileName_);

  emit message(tr("file saved") + ": " + fileName_);
  QApplication::beep();
  emit fileSaved(fileName_);
  DOUT("exit");
}

void
HttpStreamSession::finish()
{
  DOUT("enter");
  qint64 size = 0;
  if (fifo_) {
    fifo_->finish();
    size = fifo_->size();
  }

  if (isRunning()) {
    save();
    setState(Finished);
  }

  //if (reply_ && reply_->isRunning())
  //  reply_->abort();

  emit progress(size, size);

  stoppedCond_.wakeAll();
  DOUT("exit");
}

void
HttpStreamSession::stop()
{
  DOUT("enter");
  setState(Stopped);

  if (progressTask_)
    progressTask_->stop();

  foreach (InputStream *in, ins_) {
    RemoteStream *r = dynamic_cast<RemoteStream *>(in);
    if (r)
      r->stop();
  }
  quit();
  DOUT("exit");
}

void
HttpStreamSession::waitForReady()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (!ready_)
    readyCond_.wait(&m_);
  DOUT("exit");
}

void
HttpStreamSession::waitForStopped()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (isRunning())
    stoppedCond_.wait(&m_);
  DOUT("exit");
}

void
HttpStreamSession::run()
{
  DOUT("enter: urls.size =" << urls_.size() << ", urls =" << urls_);
  Q_ASSERT(!url_.isEmpty());
  if (urls_.isEmpty()) {
    DOUT("exit: empty urls");
    return;
  }
  setState(Running);
  ready_ = false;
  //progress_.clear();
  invalidateFileName();

  if (merger_) {
    delete merger_;
    merger_ = 0;
  }
  if (fifo_) {
    delete fifo_;
    fifo_ = 0;
  }
  if (!ins_.isEmpty()) {
    foreach (InputStream *in, ins_)
      delete in;
    ins_.clear();
  }

  if (mediaTitle().isEmpty())
    setMediaTitle(QDateTime::currentDateTime().toString(Qt::ISODate));

  bool ok = true;

  foreach (const QUrl &url, QtExt::revertList(urls_)) {
    RemoteStream *in = new BufferedRemoteStream;
    //QtExt::ProgressWithId p((long)in);
    //connect(in, SIGNAL(progress(qint64,qint64)), &p, SLOT(emit_progress(qint64,qint64)));
    //connect(&p, SIGNAL(progress(qint64,qint64,long)), SLOT(updateProgressWithId(qint64,qint64,long)), Qt::QueuedConnection);
    in->setUrl(url);

    if (cookieJar())
      in->networkAccessManager()->setCookieJar(cookieJar());
    in->run();
    ins_.prepend(in);
    in->waitForReady();

    QString type = in->contentType();
    ok = isMultiMediaMimeType(type);
    if (!ok) {
      DOUT("WARNING: invalid contentType =" << type << ", url =" << url.toString());
      ins_.removeFirst();
    }
  }
  if (cookieJar())
    cookieJar()->setParent(0);

  //if (reply_->error() != QNetworkReply::NoError) {
  //  setState(Error);
  //  ready_ = true;
  //  readyCond_.wakeAll();
  //  stoppedCond_.wakeAll();
  //  emit error(tr("network error to access URL") + ": " + url_.toString());
  //  DOUT("exit: network error");
  //  return;
  //}

  if (ins_.isEmpty()) {
    DOUT("access forbidden");
    setState(Error);
    emit error(tr("access forbidden") + ": " + urls_.first().toString());

    ready_ = true;
    readyCond_.wakeAll();
    stoppedCond_.wakeAll();
    DOUT("exit: error caused by invalid content type");
    return;
  }

  fifo_ = new SecureBufferedFifoStream;
  invalidateSize();

  updateProgress();

  merger_ = new FlvMerge; {
    merger_->setInputStreams(ins_);
    merger_->setOutputStream(fifo_);
    merger_->setDuration(duration_);

    connect(merger_, SIGNAL(error(QString)), SIGNAL(error(QString)));
    connect(merger_, SIGNAL(error(QString)), SLOT(stop()));
    //connect(merger_, SIGNAL(timestampChanged(qint64)), SLOT(updateProgress()));
  }

  //QTimer progressTimer;
  //progressTimer.setInterval(0); // report every second
  //connect(&progressTimer, SIGNAL(timeout()), SLOT(updateProgress()), Qt::QueuedConnection);

  //progressTimer.start();
  //merger_->run();
  //progressTimer.stop();

  //ins_.reset();
  ok = merger_->parse();
  if (ok) {
    duration_ = merger_->duration();
    DOUT("duration =" << duration_);
  } else {
    setState(Error);
    emit error(tr("failed to parse FLV streams"));
  }

  ready_ = true;
  readyCond_.wakeAll();

  if (ok) {
    progressTask_ = new ProgressTask_(this);
    QThreadPool::globalInstance()->start(progressTask_);
  }

  if (ok) {
    ins_.reset();
    ok = merger_->merge();
    if (ok)
      merger_->finish();
    else {
      setState(Error);
      emit error(tr("failed to merge FLV streams"));
    }
  }

  if (progressTask_)
    progressTask_->stop();

  if (isRunning()) {
    finish();

    DOUT("exec after finished");
  } else
    emit tr("failed to process remote media");

  DOUT("exit");
}

// EOF

/*
// - Progress -

void
HttpStreamSession::updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id)
{
  progress_[id] = Progress(receivedBytes, totalBytes);

  receivedBytes = 0;
  totalBytes = 0;
  foreach (const Progress &p, progress_) {
    receivedBytes += p.receivedBytes;
    totalBytes += p.totalBytes;
  }
  emit progress(receivedBytes, totalBytes);
}
*/