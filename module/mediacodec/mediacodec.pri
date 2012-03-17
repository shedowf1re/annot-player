# mediacodec.pri
# 2/12/2012

include(../../config.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/aaccodec.h \
    $$PWD/codecinfo.h \
    $$PWD/flvcodec.h \
    $$PWD/flvdemux.h \
    $$PWD/flvmerge.h \
    $$PWD/flvparser.h \
    $$PWD/mediatoc.h \
    $$PWD/mediawriter.h \
    $$PWD/mp4codec.h

SOURCES += \
    $$PWD/aaccodec.cc \
    $$PWD/aacwriter.cc \
    $$PWD/flvcodec.cc \
    $$PWD/flvdemux.cc \
    $$PWD/flvmerge.cc \
    $$PWD/flvparser.cc \
    $$PWD/h264writer.cc \
    $$PWD/mediatoc.cc \
    $$PWD/mp4codec.cc

QT      += core
#DEFINES += WITH_MP4V2
#LIBS    += -lmp4v2
#LIBS   += -lavcodec -lavformat -lavutil
#LIBS   += -lfaac

# EOF
