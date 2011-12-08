// ioutil.cc
// 12/6/2011

#include "ioutil.h"
#include <QtCore>

#ifdef USE_MODULE_BLOCKIODEVICE
  #include "module/blockiodevice/blockiodevice.h"
  #ifdef Q_OS_WIN
    #include "win/qtwin/qtwin.h"
  #endif // Q_OS_WIN
  #ifdef Q_OS_UNIX
    #include "unix/qtunix/qtunix.h"
  #endif // Q_OS_UNIX
#endif // USE_MODULE_BLOCKIODEVICE

#define DEBUG "IOUtil"
#include "module/debug/debug.h"

QByteArray
IOUtil::readBytes(const QString &input, qint64 maxSize)
{
  DOUT("readBytes:enter: input =" << input << ", maxSize =" << maxSize);
  QString filePath = input;
  QByteArray data;
  if (!filePath.isEmpty()) {
#ifdef USE_MODULE_BLOCKIODEVICE
#ifdef Q_OS_WIN
    QString deviceFileName = QtWin::guessDeviceFileName(filePath);
    if (QtWin::isValidDeviceFileName(deviceFileName)) {
      filePath = deviceFileName;
#elif defined (Q_OS_UNIX)
    if (QtUnix::isDeviceFile(filePath)) {
#else
    if (0) {
#endif // Q_OS_
      BlockIODevice file(filePath);
      bool ok = file.open(QIODevice::ReadOnly);
      if (!ok) {
        DOUT("readBytes:exit: Failed to open file for hashing: " << filePath);
        return QByteArray();
      }

      int readSize = maxSize;
      int blockSize = file.blockSize();
      if (blockSize > 0 && maxSize % blockSize) {
        readSize = ((maxSize / blockSize) + 1) * blockSize;
        DOUT("readBytes: blockSize =" << blockSize <<
             ", maxSize =" << maxSize <<
             ", readSize =" << readSize);
      }

      data = file.read(readSize);
      file.close();
    } else
#endif // USE_MODULE_BLOCKIODEVICE
    {
      QFile file(filePath);
      bool ok = file.open(QIODevice::ReadOnly);
      if (!ok) {
        DOUT("readBytes:exit: Failed to open file for hashing: " << filePath);
        return QByteArray();
      }

      data = file.read(maxSize);
      file.close();
    }
  }

  if (data.isEmpty()) {
    DOUT("readBytes:exit: error: get empty data to hash: " << filePath);
    return QByteArray();
  }

  if (data.size() > maxSize) {
     DOUT("readBytes: truncate from" << data.size() << " to" << maxSize);
     data.truncate(maxSize);
  }
  return data;
}
// EOF
