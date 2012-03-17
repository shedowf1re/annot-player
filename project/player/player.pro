# player.pro
# 6/30/2011

VERSION = 0.1.3.2

include(../../config.pri)
include(tr/tr.pri)

## Libraries

include($$ROOTDIR/module/annotcloud/annotcloud.pri)
include($$ROOTDIR/module/annotcodec/annotcodec.pri)
include($$ROOTDIR/module/stream/stream.pri)
include($$ROOTDIR/module/player/player.pri)
#include($$ROOTDIR/module/doll/doll.pri)
include($$ROOTDIR/module/serveragent/serveragent.pri)
#include($$ROOTDIR/module/clientagent/clientagent.pri)
include($$ROOTDIR/module/gsoap/gsoap.pri)       # would static linking cause license conflicts?
#include($$ROOTDIR/module/streamservice/streamservice.pri)
include($$ROOTDIR/module/mediacodec/mediacodec.pri)
include($$ROOTDIR/module/translator/translator.pri)
include($$ROOTDIR/module/mrlresolver/mrlresolver.pri)
#include($$ROOTDIR/module/mp4box/mp4box.pri)
include($$ROOTDIR/module/ioutil/ioutil.pri)
include($$ROOTDIR/module/nicoutil/nicoutil.pri)
include($$ROOTDIR/module/qtext/qtext.pri)
include($$ROOTDIR/module/crypt/crypt.pri)

# shared link gave me so many trouble on mac and linux
unix:       include($$ROOTDIR/module/webbrowser/webbrowser_static.pri)
win32:      include($$ROOTDIR/module/webbrowser/webbrowser.pri)

DEFINES += USE_MODULE_IOUTIL
#DEFINES += USE_MODE_DEBUG

#DEFINES += USE_MODULE_DOLL
DEFINES += USE_MODULE_SERVERAGENT
#DEFINES += USE_MODULE_CLIENTAGENT

win32 {
     DEPENDPATH += $$ROOTDIR/core
     DEPENDPATH += $$ROOTDIR/module/mediaplayer
     #DEPENDPATH += $$ROOTDIR/module/doll
}

win32 {
    include($$ROOTDIR/win/dwm/dwm.pri)
    #include($$ROOTDIR/win/hook/hook.pri)
    include($$ROOTDIR/win/mousehook/mousehook.pri)
    include($$ROOTDIR/win/picker/picker.pri)
    include($$ROOTDIR/win/qth/qth.pri)
    include($$ROOTDIR/win/qtwin/qtwin.pri)
    DEFINES += USE_WIN_DWM
    #DEFINES += USE_WIN_HOOK
    DEFINES += USE_WIN_MOUSEHOOK
    DEFINES += USE_WIN_PICKER
    DEFINES += USE_WIN_QTH

    DEPENDPATH += $$ROOTDIR/win/dwm
    #DEPENDPATH += $$ROOTDIR/win/hook
    DEPENDPATH += $$ROOTDIR/win/mousehook
    DEPENDPATH += $$ROOTDIR/win/qtwin
    DEPENDPATH += $$ROOTDIR/win/qth
}
unix: {
    include($$ROOTDIR/unix/qtunix/qtunix.pri)
}
unix:!mac {
    include($$ROOTDIR/unix/qtx/qtx.pri)
}
mac {
    include($$ROOTDIR/mac/qtmac/qtmac.pri)
    include($$ROOTDIR/mac/vlcstep/vlcstep.pri)
}

QT      += core gui sql network #webkit
#CONFIG(static): QTPLUGIN += qsqlite

# Increase heap space
#win32 {
#  QMAKE_CXXFLAGS += -Zm2000    # 2000 is the maximum according to MSDN
#  #QMAKE_LFLAGS   += -Zm1000
#}

## Sources

TEMPLATE      = app
win32:  TARGET = player
unix:   TARGET = annot-player
mac:   TARGET = "Annot Player"

win32: CONFIG += windows

SUBPATH = \
    $$PWD \
    $$PWD/annot \
    $$PWD/command \
    $$PWD/data \
    $$PWD/db \
    $$PWD/define \
    $$PWD/dialog \
    $$PWD/download \
    $$PWD/osd \
    $$PWD/player \
    $$PWD/signal \
    $$PWD/token \
    $$PWD/tr \
    $$PWD/user \
    $$PWD/util \
    $$PWD/web
INCLUDEPATH     += $$SUBPATH
DEPENDPATH      += $$SUBPATH

HEADERS += \
    application.h \
    clipboardmonitor.h \
    config.h \
    eventlogger.h \
    mainwindow.h \
    mainwindowprivate.h \
    settings.h \
    tray.h \
    uistyle.h \
    annot/annotationbrowser.h \
    annot/annotationeditor.h \
    annot/annotationfilter.h \
    annot/annotationgraphicsitem.h \
    annot/annotationgraphicsview.h \
    annot/annotationgraphicsstyle.h \
    annot/blacklistview.h \
    annot/blacklistviewprivate.h \
    annot/textformathandler.h \
    command/annotationcomboedit.h \
    command/inputcombobox.h \
    command/prefixcombobox.h \
    data/datamanager.h \
    data/dataserver.h \
    db/db_config.h \
    db/db.h \
    define/config.h \
    define/defines.h \
    define/rc.h \
    define/stylesheet.h \
    dialog/aboutdialog.h \
    dialog/annotationcountdialog.h \
    dialog/backlogdialog.h \
    dialog/consoledialog.h \
    dialog/countdowndialog.h \
    dialog/devicedialog.h \
    dialog/helpdialog.h \
    dialog/inputdialog.h \
    dialog/livedialog.h \
    dialog/logindialog.h \
    dialog/mediaurldialog.h \
    dialog/networkproxydialog.h \
    dialog/pickdialog.h \
    dialog/seekdialog.h \
    dialog/shutdowndialog.h \
    dialog/sleepdialog.h \
    dialog/siteaccountview.h \
    dialog/suburldialog.h \
    dialog/syncdialog.h \
    dialog/urldialog.h \
    download/downloaddialog.h \
    download/downloadtaskdialog.h \
    osd/osdconsole.h \
    osd/osdwindow.h \
    osd/videoview.h \
    player/mainplayer.h \
    player/miniplayer.h \
    player/embeddedplayer.h \
    player/playerpanel.h \
    player/playerui.h \
    player/userlabel.h \
    signal/signalhub.h \
    token/addaliasdialog.h \
    token/tokenview.h \
    user/userview.h \
    util/closewidgetthread.h \
    util/comboedit.h \
    util/comboeditprivate.h \
    util/filteredlistview.h \
    util/filteredtableview.h \
    util/grabber.h \
    util/lineedit.h \
    util/logger.h \
    util/textedit.h \
    util/textedittabview.h \
    util/textview.h \
    web/proxybrowser.h

SOURCES += \
    application.cc \
    clipboardmonitor.cc \
    eventlogger.cc \
    main.cc \
    mainwindow.cc \
    settings.cc \
    tray.cc \
    uistyle.cc \
    annot/annotationbrowser.cc \
    annot/annotationeditor.cc \
    annot/annotationfilter.cc \
    annot/annotationgraphicsitem.cc \
    annot/annotationgraphicsview.cc \
    annot/annotationgraphicsstyle.cc \
    annot/blacklistview.cc \
    annot/blacklistviewprivate.cc \
    annot/textformathandler.cc \
    command/annotationcomboedit.cc \
    command/inputcombobox.cc \
    command/prefixcombobox.cc \
    data/datamanager.cc \
    data/dataserver.cc \
    db/db.cc \
    dialog/aboutdialog.cc \
    dialog/annotationcountdialog.cc \
    dialog/backlogdialog.cc \
    dialog/consoledialog.cc \
    dialog/countdowndialog.cc \
    dialog/devicedialog.cc \
    dialog/helpdialog.cc \
    dialog/inputdialog.cc \
    dialog/livedialog.cc \
    dialog/logindialog.cc \
    dialog/mediaurldialog.cc \
    dialog/networkproxydialog.cc \
    dialog/pickdialog.cc \
    dialog/seekdialog.cc \
    dialog/shutdowndialog.cc \
    dialog/sleepdialog.cc \
    dialog/siteaccountview.cc \
    dialog/suburldialog.cc \
    dialog/syncdialog.cc \
    dialog/urldialog.cc \
    download/downloaddialog.cc \
    download/downloadtaskdialog.cc \
    osd/osdconsole.cc \
    osd/osdwindow.cc \
    osd/videoview.cc \
    player/mainplayer.cc \
    player/miniplayer.cc \
    player/embeddedplayer.cc \
    player/playerui.cc \
    player/playerpanel.cc \
    player/userlabel.cc \
    signal/signalhub.cc \
    token/addaliasdialog.cc \
    token/tokenview.cc \
    user/userview.cc \
    util/comboedit.cc \
    util/filteredtableview.cc \
    util/filteredlistview.cc \
    util/grabber.cc \
    util/lineedit.cc \
    util/logger.cc \
    util/textedit.cc \
    util/textedittabview.cc \
    util/textview.cc \
    web/proxybrowser.cc

win32 {
  DEFINES += USE_MODE_SIGNAL

  HEADERS += \
    signal/messagehandler.h \
    signal/messageview.h \
    signal/processinfo.h \
    signal/processview.h \
    signal/signalview.h

  SOURCES += \
    signal/messagehandler.cc \
    signal/messageview.cc \
    signal/processinfo.cc \
    signal/processview.cc \
    signal/signalview.cc
}

RESOURCES += player.qrc

OTHER_FILES += \
    annot-player.desktop \
    debian.rules \
    debian.control \
    deploy-debian.sh \
    deploy-fedora.sh \
    deploy-mac.sh \
    deploy-win.cmd \
    Info.plist \
    player.rc \
    player.ico \
    player.icns \
    rpm.spec

win32 {
    !wince*: LIBS += -lshell32
    RC_FILE += player.rc
}

mac {
    ICON = player.icns
    QMAKE_INFO_PLIST = Info.plist
}

# Deployment

unix:!mac {
    INSTALLS += target desktop desktop-kde icon

    target.path = $$BINDIR

    LUA_SCRIPTS.path = $$DATADIR/annot/player/lua

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    desktop-kde.path = $$DATADIR/kde4/apps/solid/actions
    desktop-kde.files += $${TARGET}.desktop

    #service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    icon.path = $$DATADIR/icons/hicolor/256x256/apps
    icon.files += $${TARGET}.png
}

# EOF
